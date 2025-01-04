// API响应基础接口
export interface ApiResponse<T = any> {
  code: number;
  message: string;
  data: T;
}

// 订单相关接口
export interface Order {
  orderId: string;
  createTime: string;
  totalAmount: number;
  status: OrderStatus;
  items: OrderItem[];
  comment?: Comment;
}

export interface OrderItem {
  id: string;
  name: string;
  price: number;
  quantity: number;
}

export enum OrderStatus {
  PENDING_PAYMENT = 'PENDING_PAYMENT',
  PAID = 'PAID',
  PREPARING = 'PREPARING',
  DELIVERING = 'DELIVERING',
  COMPLETED = 'COMPLETED',
  CANCELLED = 'CANCELLED'
}

// 评论相关接口
export interface Comment {
  id: string;
  username: string;
  userAvatar: string;
  rating: number;
  content: string;
  createTime: string;
  orderItems?: string[];
  merchantReply?: string;
}

// 商家相关接口
export interface Merchant {
  id: string;
  name: string;
  logo: string;
  rating: number;
  monthlyOrders: number;
  status: 'OPEN' | 'CLOSED';
  address: string;
  businessHours: string;
  phone: string;
  description: string;
}

// 仪表盘数据接口
export interface DashboardData {
  todayOrders: number;
  orderTrend: number;
  todayRevenue: number;
  revenueTrend: number;
  newUsers: number;
  userTrend: number;
  activeMerchants: number;
  merchantTrend: number;
} 