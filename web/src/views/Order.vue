<template>
  <div class="order-container">
    <header class="header">
      <h1>我的订单</h1>
    </header>

    <main class="order-content">
      <div v-if="loading" class="loading">
        加载中...
      </div>
      <div v-else-if="errorMessage" class="error-message">
        {{ errorMessage }}
      </div>
      <div v-else-if="orders.length === 0" class="empty-orders">
        暂无订单
      </div>
      <div v-else class="order-list">
        <div v-for="order in orders" :key="order.id" class="order-card">
          <div class="order-header">
            <span class="order-id">订单号: {{ order.id }}</span>
            <span class="order-status">{{ order.status }}</span>
          </div>
          <div class="order-items">
            <div v-for="item in order.items" :key="item.id" class="order-item">
              <span class="item-name">{{ item.name }}</span>
              <div class="item-details">
                <span class="item-quantity">x{{ item.quantity }}</span>
                <span class="item-price">¥{{ item.price.toFixed(2) }}</span>
              </div>
            </div>
          </div>
          <div class="order-footer">
            <span class="order-time">{{ formatTime(order.created_at) }}</span>
            <span class="order-total">总计: ¥{{ order.total.toFixed(2) }}</span>
          </div>
        </div>
      </div>
    </main>
  </div>
</template>

<script lang="ts">
import { ref, onMounted, defineComponent } from 'vue'
import api from '@/api'

interface OrderItem {
  id: number
  name: string
  price: number
  quantity: number
}

interface Order {
  id: number
  items: OrderItem[]
  total: number
  status: string
  created_at: string
}

export default defineComponent({
  name: 'Order',
  setup() {
    const orders = ref<Order[]>([])
    const loading = ref(false)
    const errorMessage = ref('')

    const formatTime = (timestamp: string) => {
      const date = new Date(timestamp)
      return date.toLocaleString('zh-CN', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit'
      })
    }

    const loadOrders = async () => {
      try {
        loading.value = true
        errorMessage.value = ''

        const response = await api.getOrders()
        if (response.error === 0) {
          orders.value = response.orders
        } else {
          errorMessage.value = response.message || '获取订单失败'
        }
      } catch (error) {
        console.error('获取订单错误:', error)
        errorMessage.value = '获取订单失败'
      } finally {
        loading.value = false
      }
    }

    onMounted(() => {
      loadOrders()
    })

    return {
      orders,
      loading,
      errorMessage,
      formatTime
    }
  }
})
</script>

<style scoped>
.order-container {
  min-height: 100vh;
  background-color: #f5f5f5;
  padding-bottom: 2rem;
}

.header {
  background-color: white;
  padding: 1rem 2rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  position: sticky;
  top: 0;
  z-index: 10;
}

.header h1 {
  margin: 0;
  color: #333;
}

.order-content {
  max-width: 800px;
  margin: 0 auto;
  padding: 2rem;
}

.loading {
  text-align: center;
  padding: 2rem;
  color: #666;
}

.error-message {
  text-align: center;
  padding: 2rem;
  color: #ff5252;
}

.empty-orders {
  text-align: center;
  padding: 2rem;
  color: #666;
}

.order-list {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.order-card {
  background: white;
  border-radius: 8px;
  overflow: hidden;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.order-header {
  padding: 1rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  background-color: #f8f9fa;
  border-bottom: 1px solid #eee;
}

.order-id {
  color: #666;
  font-family: monospace;
}

.order-status {
  padding: 0.25rem 0.5rem;
  border-radius: 4px;
  background-color: #e3f2fd;
  color: #1976d2;
  font-size: 0.9rem;
}

.order-items {
  padding: 1rem;
}

.order-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0.5rem 0;
}

.item-name {
  color: #333;
}

.item-details {
  display: flex;
  align-items: center;
  gap: 1rem;
}

.item-quantity {
  color: #666;
}

.item-price {
  color: #ff5252;
  font-weight: bold;
}

.order-footer {
  padding: 1rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  background-color: #f8f9fa;
  border-top: 1px solid #eee;
}

.order-time {
  color: #666;
  font-size: 0.9rem;
}

.order-total {
  font-weight: bold;
  color: #ff5252;
}
</style> 