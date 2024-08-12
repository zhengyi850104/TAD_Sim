// Copyright 2024 Tencent Inc. All rights reserved.
//
// Author: (dongyuanhu)
//
// 1. 获取初始帧的主车加速度的正负状态(假设正)
// 2. 获取下帧的主车加速度小于-0.5m/s^2,计一次
// 3. 获取之后帧,主车加速度大于0.5m/s^2,记一次
//
// 正->负->正, 0.3s(默认)内若出现三次正负变化,记一次顿挫
// 修正:根据分段函数,判断jerk大于阈值则记一次顿挫

#pragma once

#include "eval/eval_base.h"

namespace eval {

class EvalSetBackCheck : public EvalBase {
 private:
  /**
   * @brief 用下降沿检测模版类
   */
  FallingDetection<double> _detector = MakeFallingDetection<double>(1e6);
  int _setback_count;
  bool event_setback;
  double scene_dis;

 public:
  /**
   * @brief 指标构造函数
   */
  EvalSetBackCheck();
  /**
   * @brief 指标析构函数
   */
  virtual ~EvalSetBackCheck() {}

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
  static sim_msg::TestReport_XYPlot _s_set_back_plot;
  /**
   * @brief 指标结果附加信息，评测报告用
   */
  static sim_msg::TestReport_PairData _g_setback_pair;
  /**
   * @brief 指标名称定义，在factory中注册时的参数，且与json中"name"一一对应
   */
  static const char _kpi_name[];
  /**
   * @brief  指标Build方法，在factory中注册时传入的方法
   * @return EvalBasePtr 指标指针
   */
  static EvalBasePtr Build() { return std::make_shared<EvalSetBackCheck>(); }
};
}  // namespace eval
