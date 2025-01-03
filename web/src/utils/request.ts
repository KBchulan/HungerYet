import axios from 'axios'
import type { AxiosInstance, AxiosRequestConfig, AxiosResponse } from 'axios'

const service: AxiosInstance = axios.create({
  baseURL: 'http://localhost:8080',
  timeout: 5000,
  headers: {
    'Content-Type': 'application/json'
  }
})

service.interceptors.request.use(
  (config) => {
    console.log('发送请求:', config.url, config.data)
    const token = localStorage.getItem('token')
    if (token) {
      config.headers.Authorization = `Bearer ${token}`
    }
    return config
  },
  (error) => {
    console.error('请求配置错误:', error)
    return Promise.reject(error)
  }
)

service.interceptors.response.use(
  (response: AxiosResponse) => {
    console.log('收到响应:', response.config.url, response.data)
    return response.data
  },
  (error) => {
    console.error('响应错误:', error.config?.url, error.message)
    if (error.response) {
      console.error('错误响应数据:', error.response.data)
    }
    return Promise.reject(error)
  }
)

export default service
