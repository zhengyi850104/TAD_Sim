// Copyright 2024 Tencent Inc. All rights reserved.
//
// Author: (dongyuanhu)
//
// 1. 检测前方车辆距离在最大跟停距离内(参考跟停距离2.5m~4m)
// 2. 检测前车为stopped状态且加速度大于0.1m/s^2,记录当前时间
// 3. 检测主车加速度是否大于0.1m/s^2,起步成功
//
// 计算2/3前后时间差即起步时间,若小于预设阈值,则检测通过

#pragma once

#include "eval/eval_base.h"

namespace eval {

class EvalReactTimeOfFollStart : public EvalBase {
 private:
  /**
   * @brief 用上升沿检测模版类
   */
  RiseUpDetection<double> _detector = MakeRiseUpDetection<double>(-1e6);
  bool _event_happen;
  double _react_time;

 public:
  /**
   * @brief 指标构造函数
   */
  EvalReactTimeOfFollStart();
  /**
   * @brief 指标析构函数
   */
  virtual ~EvalReactTimeOfFollStart() {}

 public:
  /**
   * @brief 指标Init方法
   * @param helper 评测辅助类Init类
   * @return true 初始化成功
   * @return false 初始化失败
   */
  virtual bool Init(eval::EvalInit &helper);
  /**
   * @brief 指标Step方法
   * @param helper 评测辅助类Step类
   * @return true 单步执行成功
   * @return false 单步执行失败
   */
  virtual bool Step(eval::EvalStep &helper);
  /**
   * @brief 指标Stop方法
   * @param helper 评测辅助类Stop类
   * @return true 停止成功
   * @return false 停止失败
   */
  virtual bool Stop(eval::EvalStop &helper);
  /**
   * @brief 指标评测结果写入方法
   * @param msg Grading评测消息
   */
  virtual void SetGradingMsg(sim_msg::Grading &msg) override;
  /**
   * @brief 评测通过条件，判断指标是否通过
   * @return EvalResult 判断结果
   */
  virtual EvalResult IsEvalPass();
  /**
   * @brief 结束场景条件，判断指标是否应该立即停止场景
   * @param[out] reason 如果需要终止场景的原因
   * @return true 结束场景
   * @return false 不结束场景
   */
  virtual bool ShouldStopScenario(std::string &reason);

 public:
  /**
   * @brief 指标plot图定义，评测报告用
   */
  static sim_msg::TestReport_XYPlot _s_react_follow_start_plot;
  /**
   * @brief 指标名称定义，在factory中注册时的参数，且与json中"name"一一对应
   */
  static const char _kpi_name[];
  /**
   * @brief  指标Build方法，在factory中注册时传入的方法
   * @return EvalBasePtr 指标指针
   */
  static EvalBasePtr Build() { return std::make_shared<EvalReactTimeOfFollStart>(); }
};
}  // namespace eval
