# IoTEventMonitorPlatform
物联网事件监控平台系统

# conf
    - gui_conf.xml 页面配置
    - gui_conf_default.xml 默认页面配置
# output
仅在构建后生成，包含输出的可执行文件
# src
    - GUI 图形界面部分，使用QT5编写
    - manager 后台线程，收集事件、验证事件
    - netfilterClient 使用netlink与内核netfilter作事件交互
    - serialPortClient 串口中继器，从串口获取事件
    - tinyxml2 开源的xml解析工具
    - verification 事件验证模块
        - rvsDFAInterface 状态机抽象接口
        - rvsDFA 具体的状态机