//
// Created by chenkuan on 2018/2/20.
//

#include <GUIConf.h>

// 配置文件位置
const char *GUI_CONF_FILE = "../conf/gui_conf.xml";
// 默认配置文件位置
const char *GUI_CONF_DEFAULT_FILE = "../conf/gui_conf_default.xml";
// 默认配置内容
const char *GUI_CONF_TEMPLATE = "<xml projectName=\"IoTEventMonitorPlatform\">\n</xml>";

// 配置中节点信息
const char *ROOT_TAG = "xml";
const char *PROJECT_NAME_ATTR = "projectName";
const char *PROJECT_NAME_ATTR_VALUE = "IoTEventMonitorPlatform";

const char *EVENTS_TAG = "events";
const char *CURRENT_EVENT_NAME_ATTR = "currentEventName";
const char *EVENT_TAG = "event";
const char *EVENT_NAME_ATTR = "name";
const char *EVENT_HEAD_TAG = "head";
const char *EVENT_BODY_TAG = "body";
const char *EVENT_TAIL_TAG = "tail";

const char *RUN_TAG = "run";
const char *VM_IP_ATTR = "vmIP";
const char *EXTERNAL_IP_ATTR = "externalIP";
const char *PSEUDO_TERMINAL_ATTR = "pseudoTerminal";
const char *SERIAL_PORT_ATTR = "serialPort";
const char *STATE_MACHINE_FILE_PATH_ATTR = "stateMachineFilePath";

// 尺寸
const int MAIN_WINDOW_WIDTH = 1000;
const int MAIN_WINDOW_HEIGHT = 600;

const int MAIN_TAB_HEIGHT = 40;

const int EVENT_TAB_WIDTH = 100;
const int EVENT_TAB_HEIGHT = 40;

const char *ADD_EVENT = "+";
const char *DELETE_EVENT = "-";

const char *DEFAULT_EVENT_MATCH_HEAD = "<xml>";
const char *DEFAULT_EVENT_MATCH_BODY = "<name>value</name>";
const char *DEFAULT_EVENT_MATCH_TAIL = "</xml>";

#ifdef CHINESE
const char *MAIN_WINDOW_TITLE = "物联网服务系统监控平台";

const char *EVENT_MANAGER_TITLE = "事件模板定义";
const char *EVENT = "事件";
const char *EVENT_MATCH_HEAD_DESC = "事件匹配头部";
const char *EVENT_MATCH_BODY_DESC = "事件非匹配主体";
const char *EVENT_MATCH_TAIL_DESC = "事件匹配尾部";

const char *RUN_TITLE = "运行展示";
const char *EVENT_SELECT_LABEL = "事件选择";
const char *VM_IP_LABEL = "虚拟机ip";
const char *EXTERNAL_IP_LABEL = "外部设备ip";
const char *PSEUDO_TERMINAL_LABEL = "KVM伪终端名称";
const char *HOST_SERIAL_PORT_LABEL = "宿主机串口名称";
const char *STATE_MACHINE_SELECT_LABEL = "状态机文件选择";
const char *RUN_BUTTON = "启动";
const char *CLEAR_BUTTON = "清屏";

const char *SELECT_FILE_DIALOG_TITLE = "选择文件";
#endif

#ifdef ENGLISH
const char *MAIN_WINDOW_TITLE = "IoT service system monitor platform";
const char *EVENT_MANAGER_TITLE = "event template define";
const char *RUN_TITLE = "run & show";
#endif