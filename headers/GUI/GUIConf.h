//
// Created by chenkuan on 2017/2/20.
//

#ifndef IOTEVENTMONITORPLATFORM_GUISTYLE_H
#define IOTEVENTMONITORPLATFORM_GUISTYLE_H

// 配置文件位置
extern const char *GUI_CONF_FILE;
// 默认配置文件位置
extern const char *GUI_CONF_DEFAULT_FILE;
// 默认配置内容
extern const char *GUI_CONF_TEMPLATE;

// 配置中节点信息
extern const char *ROOT_TAG;
extern const char *PROJECT_NAME_ATTR;
extern const char *PROJECT_NAME_ATTR_VALUE;
extern const char *EVENTS_TAG;
extern const char *EVENT_TAG;
extern const char *EVENT_NAME_ATTR;
extern const char *EVENT_HEAD_TAG;
extern const char *EVENT_BODY_TAG;
extern const char *EVENT_TAIL_TAG;
extern const char *RUN_TAG;
extern const char *CURRENT_EVENT_NAME_ATTR;
extern const char *VM_IP_ATTR;
extern const char *EXTERNAL_IP_ATTR;
extern const char *PSEUDO_TERMINAL_ATTR;
extern const char *SERIAL_PORT_ATTR;
extern const char *STATE_MACHINE_FILE_PATH_ATTR;

// 尺寸信息
extern const int MAIN_WINDOW_WIDTH;
extern const int MAIN_WINDOW_HEIGHT;

extern const int MAIN_TAB_HEIGHT;

extern const int EVENT_TAB_WIDTH;
extern const int EVENT_TAB_HEIGHT;

extern const char *ADD_EVENT;
extern const char *DELETE_EVENT;

extern const char *DEFAULT_EVENT_MATCH_HEAD;
extern const char *DEFAULT_EVENT_MATCH_BODY;
extern const char *DEFAULT_EVENT_MATCH_TAIL;

// 界面显示语言选择
#define CHINESE

extern const char *MAIN_WINDOW_TITLE;

extern const char *EVENT_MANAGER_TITLE;
extern const char *EVENT;
extern const char *EVENT_MATCH_HEAD_DESC;
extern const char *EVENT_MATCH_BODY_DESC;
extern const char *EVENT_MATCH_TAIL_DESC;

extern const char *RUN_TITLE;
extern const char *EVENT_SELECT_LABEL;
extern const char *VM_IP_LABEL;
extern const char *EXTERNAL_IP_LABEL;
extern const char *PSEUDO_TERMINAL_LABEL;
extern const char *HOST_SERIAL_PORT_LABEL;
extern const char *STATE_MACHINE_SELECT_LABEL;
extern const char *RUN_BUTTON;
extern const char *CLEAR_BUTTON;

extern const char *SELECT_FILE_DIALOG_TITLE;

#endif //IOTEVENTMONITORPLATFORM_GUISTYLE_H
