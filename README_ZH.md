# 用户身份凭据管理

- [简介](#简介)
- [目录](#目录)
- [说明](#说明)
  - [接口说明](#接口说明)
  - [使用说明](#使用说明)
- [相关仓](#相关仓)


## 简介

**用户身份凭据管理**（useridm）是用户IAM子系统的基础部件之一，向上提供系统内统一的用户身份凭据信息管理（设置、修改和删除）接口，向下通过认证执行器管理模块，调用系统内的认证资源，完成用户身份凭据的生命周期管理和安全存储。

**图1** 用户身份凭据管理架构图

<img src="figures/用户身份凭据管理架构图.png" alt="用户身份凭据管理架构图" style="zoom:80%;" />

## 目录

```undefined
//base/user_iam/user_idm
├── frameworks			# 框架代码
├── interfaces			# 对外接口存放目录
│   └── innerkits		# 对内部子系统暴露的头文件，供系统服务使用
├── sa_profile			# Service Ability 配置文件
├── services			# Service Ability 服务实现
├── unittest			# 测试代码存放目录
├── utils				# 工具代码存放目录
├── bundle.json			# 组件描述文件
└── useridm.gni			# 构建配置
```


## 说明

### 接口说明

**表1** 凭据信息管理接口

| 接口名  | 描述                             |
| ------ | -------------------------------- |
| addCredential(credentialInfo : CredentialInfo, callback : IIdmCallback) : void; | 凭据录入接口 |
| updateCredential(credentialInfo:CredentialInfo, callback:IIdmCallback) : void; | 凭据修改接口 |
| delUser(token : Uint8Array, callback : IIdmCallback) : void; | 口令删除接口，当系统内删除用户口令时，也同时删除该用户的全部认证凭据 |
| delCred(credentialId : Uint8Array, token : Uint8Array, callback : IIdmCallback) : void; | 凭据删除接口 |

**表2** 凭据信息查询接口

| 接口名 | 描述                       |
| ------ | -------------------------------- |
| getAuthInfo(callback : AsyncCallback<Array<EnrolledCredInfo>>,authType? : AuthType) : void; | 信息查询 接口，支持查询用户录入的一类或全部类型认证凭据 |

### 使用说明

- 需在可信执行环境内实现[useriam_auth_executor_mgr](https://gitee.com/openharmony-sig/useriam_coauth)仓内，头文件```common\interface\useridm_interface.h``` 中定义的接口，保证用户身份认证凭据信息与用户id的关联关系不可篡改，可以支持统一用户认证功能。
- OpenHarmony框架提供了相关功能的软件实现，可供厂商适配时参考实现。

## 相关仓

[useriam_auth_executor_mgr](https://gitee.com/openharmony/useriam_auth_executor_mgr)

**[useriam_user_idm](https://gitee.com/openharmony/useriam_user_idm)**

[useriam_user_auth](https://gitee.com/openharmony/useriam_user_auth)

[useriam_pin_auth](https://gitee.com/openharmony/useriam_pin_auth)

[useriam_faceauth](https://gitee.com/openharmony/useriam_faceauth)
