import axios, { AxiosInstance, AxiosResponse } from 'axios';
import { ref } from 'vue';
import { ApiResponse } from '../types/api';

// 定义请求ID类型
export enum ReqId {
  LOGIN = 'LOGIN',
  REGISTER = 'REGISTER',
  RESET_PASSWORD = 'RESET_PASSWORD',
  ADMIN = 'ADMIN',
  ORDER = 'ORDER',
  MERCHANT = 'MERCHANT',
}

// 定义模块类型
export enum Modules {
  LOGINMOD = 'LOGINMOD',
  REGISTERMOD = 'REGISTERMOD',
  RESETMOD = 'RESETMOD',
  ADMINMOD = 'ADMINMOD',
  ORDERMOD = 'ORDERMOD',
  MERCHANTMOD = 'MERCHANTMOD',
}

// 定义错误码
export enum ErrorCodes {
  SUCCESS = 'SUCCESS',
  ERR_NETWORK = 'ERR_NETWORK',
  ERR_UNAUTHORIZED = 'ERR_UNAUTHORIZED',
  ERR_FORBIDDEN = 'ERR_FORBIDDEN',
  ERR_NOT_FOUND = 'ERR_NOT_FOUND',
  ERR_SERVER = 'ERR_SERVER',
}

export default class HttpManager {
  private static instance: HttpManager;
  private eventListeners: Map<string, Array<(...args: any[]) => void>>;
  private baseURL: string = '/api';

  private constructor() {
    this.eventListeners = new Map();
  }

  public static getInstance(): HttpManager {
    if (!HttpManager.instance) {
      HttpManager.instance = new HttpManager();
    }
    return HttpManager.instance;
  }

  public on(event: string, listener: (...args: any[]) => void) {
    if (!this.eventListeners.has(event)) {
      this.eventListeners.set(event, []);
    }
    this.eventListeners.get(event)?.push(listener);
  }

  public off(event: string, listener: (...args: any[]) => void) {
    const listeners = this.eventListeners.get(event);
    if (listeners) {
      const index = listeners.indexOf(listener);
      if (index !== -1) {
        listeners.splice(index, 1);
      }
    }
  }

  private emit(event: string, ...args: any[]) {
    const listeners = this.eventListeners.get(event);
    if (listeners) {
      listeners.forEach(listener => listener(...args));
    }
  }

  public async postHttpReq(path: string, data: any, reqId: ReqId, mod: Modules) {
    console.log('Sending request:', {
      url: `${this.baseURL}${path}`,
      data,
      reqId,
      mod
    });

    try {
      const response = await axios.post(`${this.baseURL}${path}`, data);
      console.log('Response received:', {
        status: response.status,
        data: response.data
      });
      this.emit('httpFinished', reqId, JSON.stringify(response.data), ErrorCodes.SUCCESS, mod);
    } catch (error: any) {
      console.error('HTTP request failed:', {
        error,
        url: `${this.baseURL}${path}`,
        data,
        status: error.response?.status,
        statusText: error.response?.statusText,
        responseData: error.response?.data,
        errorMessage: error.message,
        stack: error.stack
      });
      
      if (error.response) {
        // 服务器响应了，但状态码不在 2xx 范围内
        console.log('Server responded with error:', error.response.data);
        this.emit('httpFinished', reqId, JSON.stringify(error.response.data), error.response.status, mod);
      } else if (error.request) {
        // 请求已经发出，但没有收到响应
        console.log('No response received from server');
        this.emit('httpFinished', reqId, '网络错误：服务器没有响应', ErrorCodes.ERR_NETWORK, mod);
      } else {
        // 发送请求时出现了错误
        console.log('Error creating request:', error.message);
        this.emit('httpFinished', reqId, `请求错误：${error.message}`, ErrorCodes.ERR_NETWORK, mod);
      }
    }
  }
} 