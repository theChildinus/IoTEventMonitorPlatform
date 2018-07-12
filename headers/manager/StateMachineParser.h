//
// Created by chenkuan on 17-11-15.
//

#ifndef IOTEVENTMONITORPLATFORM_STATEPARSER_H
#define IOTEVENTMONITORPLATFORM_STATEPARSER_H

#include <string>
#include <Model.h>
#include <Logger.h>

using std::string;

class StateMachineParser {
  public:
    /**
     * 读取由mxGraph存储的状态机xml描述文件
     * @param filename
     * @return 返回构造的rvsDFA模型，构造出错返回nullptr
     */
    static Model *initFromFile(const char *filename);

  private:
    static Model *model;
    /**
     * mxCell的id与节点实际id的映射
     */
    static map<int, int> idMap;

    /**
     * 解析变量声明的原始格式字符串，提取出变量和其类型
     * 添加到模型中
     * @param varDecl
     * @return
     */
    static bool parseVarDecl(const char *varDecl);
    /**
     * 解析状态结点上的原始格式字符串，提取出节点编号和表达式
     * 添加到模型中
     * @param state
     * @param stateGraphId 表示图的xml中节点mxcell的id
     * @return
     */
    static bool parseState(const char *state, const char *stateGraphId, bool isEndState);
    /**
     * 解析转移上的原始格式字符串，提取出转移名称和表达式
     * 添加到模型中
     * @param tran
     * @param source
     * @param target
     * @return
     */
    static bool parseTran(const char *tran, const char *source, const char *target);
    /**
     * 解析验证原始字符串，提取表达式
     * 添加到模型中
     * @param spec
     * @return
     */
    static bool parseSpec(const char *spec);

    static Logger *logger;
};


#endif //IOTEVENTMONITORPLATFORM_STATEPARSER_H
