import request from '@/utils/request'

// 接口响应类型定义
interface BaseResponse {
  error: number
  message?: string
}

interface LoginResponse extends BaseResponse {
  uid: number
  token: string
  host: string
  port: string
}

interface RegisterResponse extends BaseResponse {
  uid: number
}

interface VerifyCodeResponse extends BaseResponse {
  code: string
}

interface MenuResponse extends BaseResponse {
  menu_items: Array<{
    id: number
    name: string
    price: number
    description: string
    image_url: string
  }>
}

interface OrderResponse extends BaseResponse {
  order_id: number
}

// API 函数
export default {
  // 用户认证相关
  login(data: { email: string; password: string; type: number }) {
    return request.post<LoginResponse>('/api/user_login', data)
  },

  register(data: { email: string; password: string; code: string }) {
    return request.post<RegisterResponse>('/api/user_register', data)
  },

  resetPassword(data: { email: string; password: string; code: string }) {
    return request.post<BaseResponse>('/api/reset_pwd', data)
  },

  getVerifyCode(data: { email: string }) {
    return request.post<VerifyCodeResponse>('/api/get_varifycode', data)
  },

  // 菜单相关
  getMenu() {
    return request.get<MenuResponse>('/api/get_menu')
  },

  // 订单相关
  createOrder(data: { items: Array<{ id: number; quantity: number }> }) {
    return request.post<OrderResponse>('/api/create_order', data)
  },

  getOrders() {
    return request.get<{
      error: number
      orders: Array<{
        id: number
        items: Array<{
          id: number
          name: string
          price: number
          quantity: number
        }>
        total: number
        status: string
        created_at: string
      }>
    }>('/api/get_orders')
  }
}
