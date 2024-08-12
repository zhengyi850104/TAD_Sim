// Copyright 2024 Tencent Inc. All rights reserved.
//

#include "eval_pred_speed_avg.h"
#include "trajectory.pb.h"
#include "utils/proto_helper.h"

namespace eval {
const char EvalPredSpeedAvg::_kpi_name[] = "Prediction_SpeedErrorAverage";

sim_msg::TestReport_XlsxSheet EvalPredSpeedAvg::s_lateral_sheet;
sim_msg::TestReport_XlsxSheet_SheetData EvalPredSpeedAvg::s_index;
sim_msg::TestReport_XlsxSheet_SheetData EvalPredSpeedAvg::s_t;
sim_msg::TestReport_XlsxSheet_SheetData EvalPredSpeedAvg::s_fellow_id;
sim_msg::TestReport_XlsxSheet_SheetData EvalPredSpeedAvg::s_error;

EvalPredSpeedAvg::EvalPredSpeedAvg() { VLOG_0 << "eval algorithm " << _kpi_name << " constructed.\n"; }
bool EvalPredSpeedAvg::Init(eval::EvalInit &helper) {
  // determine whether the module is valid. If valid, check if kpi is enabled and get the threshold values.
  if (IsModuleValid()) {
    m_KpiEnabled = helper.getGradingKpiByName(_kpi_name, m_Kpi);
    m_defaultThreshDouble = getThreshValueByID_Double(m_Kpi, m_KpiEnabled);
    // output indicator configuration details
    DebugShowKpi();
  }

  // set report info
  if (isReportEnabled()) {
    ReportHelper::SetCaseInfo(_case, m_Kpi);
    ReportHelper::ConfigSheetData(s_index, "index");
    ReportHelper::ConfigSheetData(s_t, "t[s]");
    ReportHelper::ConfigSheetData(s_fellow_id, "fellow id");
    ReportHelper::ConfigSheetData(s_error, "average speed error[m]");
    ReportHelper::ConfigXLSXSheet(s_lateral_sheet, "average prediction speed error", "");
  }

  // reset fellow manager
  m_fellow_mgr.Reset();

  m_pred_payload = "";
  m_fellow_payload = "";

  m_timewindow = getThreshValueByID_Double(m_Kpi, m_KpiEnabled, "TimeWindow");
  m_timewindow = m_timewindow / 1000.0;  // to second

  m_index = 0;

  return true;
}
bool EvalPredSpeedAvg::Step(eval::EvalStep &helper) {
  // check whether the module is valid and whether the threshold is nonzero
  if (IsModuleValid() && m_defaultThreshDouble) {
    sim_msg::Predictions predictions;
    EvalMsg &&pred_msg = _msg_mgr->Get(topic::PREDICTIONS);
    EvalMsg &&traffic_msg = _msg_mgr->Get(topic::TRAFFIC);

    // push prediction message into queue when new prediction arrived
    if (pred_msg.GetPayload().size() > 0 && m_pred_payload.compare(pred_msg.GetPayload()) != 0 &&
        predictions.ParseFromString(pred_msg.GetPayload())) {
      for (auto i = 0; i < predictions.obs_size(); ++i) {
        sim_msg::Prediction prediction = predictions.obs().at(i);
        Prediction2ENU(prediction);  // convert to enu
        FellowTrajInfoPtr fellow_info = std::make_shared<FellowTrajInfo>();
        if (fellow_info) {
          fellow_info->m_t = helper.GetSimTime();
          fellow_info->m_id = prediction.id();
          fellow_info->m_prediction.Swap(&prediction);  // swap
          m_fellow_mgr.Add(fellow_info);                // push queue
        }
      }
      m_pred_payload = pred_msg.GetPayload();
    }

    // update actual trajectory of each fellow
    if (traffic_msg.GetPayload().size() > 0 && m_fellow_payload.compare(traffic_msg.GetPayload()) != 0) {
      // get all traffic vehicle actors
      VehilceActorList fellows = _actor_mgr->GetFellowActorsByType<CVehicleActorPtr>(Actor_Vehicle);
      for (auto fellow : fellows) {
        // update queue
        auto fellow_ptr = std::make_shared<CDynamicActor>();
        if (fellow_ptr) {
          fellow_ptr->CopyFrom(*fellow);
          m_fellow_mgr.Add(fellow->GetID(), fellow_ptr);
        }
      }
      m_fellow_payload = traffic_msg.GetPayload();
    }

    // filter fellow trajecoty
    std::vector<FellowTrajInfoPtr> fellow_infos = m_fellow_mgr.FilterFellows(helper.GetSimTime(), m_timewindow);
    for (auto fellow_info : fellow_infos) {
      double avg_error = 0.0;
      size_t counter = 0;
      for (auto i = 0; fellow_info && i < fellow_info->m_actual_traj.size(); ++i) {
        auto &actual_pt = fellow_info->m_actual_traj.at(i);  // actual trajectory point
        int index = GetNearestByTime(fellow_info->m_prediction, actual_pt->GetSimTime().GetSecond());
        if (0 <= index && index < fellow_info->m_actual_traj.size()) {
          // prediction point
          const sim_msg::TrajectoryPoint &prediction_pt = fellow_info->m_prediction.point().at(index);
          CDynamicActor &&pred_pt = TrajPoint2DynActor(prediction_pt);

          // sum error
          avg_error += std::abs(actual_pt->GetSpeed().GetNormal2D() - pred_pt.GetSpeed().GetNormal2D());
          counter++;
        }
      }

      // calculate average error
      avg_error = counter > 0 ? avg_error / counter : 0.0;

      if (avg_error > m_defaultThreshDouble && m_defaultThreshDouble > 0.5) {
        if (isReportEnabled()) {
          s_index.add_data()->assign(std::to_string(m_index).c_str());
          s_t.add_data()->assign(std::to_string(fellow_info->m_t).c_str());
          s_fellow_id.add_data()->assign(std::to_string(fellow_info->m_id).c_str());
          s_error.add_data()->assign(std::to_string(avg_error).c_str());
        }
        m_index++;
      }

      _detector.Detect(avg_error, m_defaultThreshDouble);
    }

    VLOG_2 << "fellow trajectory size:" << m_fellow_mgr.Size() << ".\n";
  } else {
    VLOG_0 << _kpi_name << " not enabled.\n";
    return false;
  }

  return true;
}
bool EvalPredSpeedAvg::Stop(eval::EvalStop &helper) {
  // add report
  if (isReportEnabled()) {
    ReportHelper::AddSheetData2XLSX(s_lateral_sheet, s_index);
    ReportHelper::AddSheetData2XLSX(s_lateral_sheet, s_t);
    ReportHelper::AddSheetData2XLSX(s_lateral_sheet, s_fellow_id);
    ReportHelper::AddSheetData2XLSX(s_lateral_sheet, s_error);
    ReportHelper::AddXLSXSheet2Attach(*_case.add_steps()->add_attach(), s_lateral_sheet);
  }

  return true;
}

void EvalPredSpeedAvg::SetGradingMsg(sim_msg::Grading &msg) {
  // set detected event
  EvalHelper::SetDetectedEvent(msg, _detector, _kpi_name);
}

EvalResult EvalPredSpeedAvg::IsEvalPass() {
  if (m_KpiEnabled) {
    _case.mutable_info()->set_detected_count(_detector.GetCount());
    if (_detector.GetCount() >= m_Kpi.passcondition().value() && m_Kpi.passcondition().value() >= 0.5) {
      return EvalResult(sim_msg::TestReport_TestState_FAIL, "above prediction speed error");
    } else {
      return EvalResult(sim_msg::TestReport_TestState_PASS, "prediction speed error check pass");
    }
  }

  return EvalResult(sim_msg::TestReport_TestState_PASS, "prediction speed error check skipped");
}
bool EvalPredSpeedAvg::ShouldStopScenario(std::string &reason) {
  auto ret = _detector.GetCount() >= m_Kpi.finishcondition().value() && m_Kpi.finishcondition().value() >= 0.5;
  if (ret) reason = "above prediction speed error";
  _case.mutable_info()->set_request_stop(ret);
  return ret;
}
}  // namespace eval
