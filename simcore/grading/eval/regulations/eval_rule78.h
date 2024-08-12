// Copyright 2024 Tencent Inc. All rights reserved.
//
// 第七十八条
// 高速公路应当标明车道的行驶速度, 最高车速不得超过每小时120公里, 最低车速不得低于每小时60公里.
// 在高速公路上行驶的小型载客汽车最高车速不得超过每小时120公里, 其他机动车不得超过每小时100公里,
// 摩托车不得超过每小时80公里. 同方向有2条车道的, 左侧车道的最低车速为每小时100公里; 同方向有3条以上车道的,
// 最左侧车道的最低车速为每小时110公里, 中间车道的最低车速为每小时90公里.
// 道路限速标志标明的车速与上述车道行驶车速的规定不一致的, 按照道路限速标志标明的车速行驶.
//
// \par References:
// [1] http://www.gov.cn/zhengce/2020-12/27/content_5574617.htm
//

#pragma once

#include "eval/eval_base.h"

namespace eval {
class EvalRule78 : public EvalBase {
 private:
  /**
   * @brief 用上升沿检测模版类
   */
  RiseUpDetection<int> _detector_upper_speed_m_s = MakeRiseUpDetection<int>(-1e6);
  /**
   * @brief 用下降沿检测模版类
   */
  FallingDetection<double> _detector_lower_speed_m_s = MakeFallingDetection<double>(1e12);

 public:
  /**
   * @brief 指标构造函数
   */
  EvalRule78();
  /**
   * @brief 指标析构函数
   */
  virtual ~EvalRule78() {}

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
  static sim_msg::TestReport_XYPlot s_rule78_plot;
  /**
   * @brief 指标结果附加信息，评测报告用
   */
  static sim_msg::TestReport_PairData s_speed_variance_pair;
  /**
   * @brief 指标名称定义，在factory中注册时的参数，且与json中"name"一一对应
   */
  static const char _kpi_name[];
  /**
   * @brief  指标Build方法，在factory中注册时传入的方法
   * @return EvalBasePtr 指标指针
   */
  static EvalBasePtr Build() { return std::make_shared<EvalRule78>(); }
};
}  // namespace eval
