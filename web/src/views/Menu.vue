<template>
  <div class="menu-container">
    <header class="header">
      <h1>菜单</h1>
      <div class="cart-button" @click="showCart = true">
        <span class="cart-icon">🛒</span>
        <span class="cart-count" v-if="cartItemCount > 0">{{ cartItemCount }}</span>
      </div>
    </header>

    <main class="menu-content">
      <div class="menu-grid">
        <div v-for="item in menuItems" :key="item.id" class="menu-item">
          <div class="item-image">
            <img :src="item.image_url" :alt="item.name">
          </div>
          <div class="item-info">
            <h3>{{ item.name }}</h3>
            <p class="description">{{ item.description }}</p>
            <div class="price-action">
              <span class="price">¥{{ item.price.toFixed(2) }}</span>
              <div class="quantity-control">
                <button 
                  class="quantity-button"
                  @click="decreaseQuantity(item)"
                  :disabled="!getCartQuantity(item.id)"
                >-</button>
                <span class="quantity">{{ getCartQuantity(item.id) }}</span>
                <button 
                  class="quantity-button"
                  @click="increaseQuantity(item)"
                >+</button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </main>

    <!-- 购物车弹窗 -->
    <div class="cart-modal" v-if="showCart">
      <div class="cart-content">
        <header class="cart-header">
          <h2>购物车</h2>
          <button class="close-button" @click="showCart = false">×</button>
        </header>
        
        <div class="cart-items" v-if="cartItems.length > 0">
          <div v-for="item in cartItems" :key="item.id" class="cart-item">
            <div class="item-info">
              <h4>{{ item.name }}</h4>
              <span class="price">¥{{ item.price.toFixed(2) }}</span>
            </div>
            <div class="quantity-control">
              <button 
                class="quantity-button"
                @click="decreaseQuantity(item)"
              >-</button>
              <span class="quantity">{{ item.quantity }}</span>
              <button 
                class="quantity-button"
                @click="increaseQuantity(item)"
              >+</button>
            </div>
          </div>
        </div>
        <div class="empty-cart" v-else>
          购物车是空的
        </div>

        <footer class="cart-footer">
          <div class="total">
            总计: ¥{{ totalAmount.toFixed(2) }}
          </div>
          <button 
            class="checkout-button"
            @click="handleCheckout"
            :disabled="cartItems.length === 0 || loading"
          >
            {{ loading ? '提交中...' : '提交订单' }}
          </button>
        </footer>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import { ref, computed, onMounted, defineComponent } from 'vue'
import { useRouter } from 'vue-router'
import api from '@/api'

interface MenuItem {
  id: number
  name: string
  price: number
  description: string
  image_url: string
  quantity?: number
}

export default defineComponent({
  name: 'Menu',
  setup() {
    const router = useRouter()
    const menuItems = ref<MenuItem[]>([])
    const cartItems = ref<MenuItem[]>([])
    const showCart = ref(false)
    const loading = ref(false)
    const errorMessage = ref('')

    const cartItemCount = computed(() => {
      return cartItems.value.reduce((total, item) => total + item.quantity!, 0)
    })

    const totalAmount = computed(() => {
      return cartItems.value.reduce((total, item) => total + item.price * item.quantity!, 0)
    })

    const getCartQuantity = (id: number) => {
      const item = cartItems.value.find(item => item.id === id)
      return item ? item.quantity : 0
    }

    const increaseQuantity = (item: MenuItem) => {
      const cartItem = cartItems.value.find(i => i.id === item.id)
      if (cartItem) {
        cartItem.quantity!++
      } else {
        cartItems.value.push({ ...item, quantity: 1 })
      }
    }

    const decreaseQuantity = (item: MenuItem) => {
      const index = cartItems.value.findIndex(i => i.id === item.id)
      if (index > -1) {
        if (cartItems.value[index].quantity! > 1) {
          cartItems.value[index].quantity!--
        } else {
          cartItems.value.splice(index, 1)
        }
      }
    }

    const handleCheckout = async () => {
      try {
        loading.value = true
        errorMessage.value = ''

        const items = cartItems.value.map(item => ({
          id: item.id,
          quantity: item.quantity!
        }))

        const response = await api.createOrder({ items })

        if (response.error === 0) {
          // 下单成功，跳转到订单页
          router.push('/order')
        } else {
          errorMessage.value = response.message || '下单失败，请重试'
        }
      } catch (error) {
        console.error('下单错误:', error)
        errorMessage.value = '下单失败，请重试'
      } finally {
        loading.value = false
      }
    }

    onMounted(async () => {
      try {
        loading.value = true
        const response = await api.getMenu()
        if (response.error === 0) {
          menuItems.value = response.menu_items
        } else {
          errorMessage.value = response.message || '获取菜单失败'
        }
      } catch (error) {
        console.error('获取菜单错误:', error)
        errorMessage.value = '获取菜单失败'
      } finally {
        loading.value = false
      }
    })

    return {
      menuItems,
      cartItems,
      showCart,
      loading,
      errorMessage,
      cartItemCount,
      totalAmount,
      getCartQuantity,
      increaseQuantity,
      decreaseQuantity,
      handleCheckout
    }
  }
})
</script>

<style scoped>
.menu-container {
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

.cart-button {
  position: relative;
  cursor: pointer;
  padding: 0.5rem;
}

.cart-icon {
  font-size: 1.5rem;
}

.cart-count {
  position: absolute;
  top: 0;
  right: 0;
  background-color: #ff5252;
  color: white;
  border-radius: 50%;
  padding: 0.2rem 0.5rem;
  font-size: 0.8rem;
}

.menu-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 2rem;
}

.menu-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
  gap: 2rem;
}

.menu-item {
  background: white;
  border-radius: 8px;
  overflow: hidden;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.item-image {
  width: 100%;
  height: 200px;
  overflow: hidden;
}

.item-image img {
  width: 100%;
  height: 100%;
  object-fit: cover;
}

.item-info {
  padding: 1rem;
}

.item-info h3 {
  margin: 0 0 0.5rem;
  color: #333;
}

.description {
  color: #666;
  margin-bottom: 1rem;
  font-size: 0.9rem;
}

.price-action {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.price {
  font-weight: bold;
  color: #ff5252;
}

.quantity-control {
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.quantity-button {
  width: 24px;
  height: 24px;
  border-radius: 50%;
  border: none;
  background-color: #4CAF50;
  color: white;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1rem;
}

.quantity-button:disabled {
  background-color: #ccc;
  cursor: not-allowed;
}

.quantity {
  min-width: 24px;
  text-align: center;
}

.cart-modal {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(0,0,0,0.5);
  display: flex;
  justify-content: flex-end;
  z-index: 100;
}

.cart-content {
  background: white;
  width: 100%;
  max-width: 400px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.cart-header {
  padding: 1rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  border-bottom: 1px solid #eee;
}

.cart-header h2 {
  margin: 0;
}

.close-button {
  background: none;
  border: none;
  font-size: 1.5rem;
  cursor: pointer;
  color: #666;
}

.cart-items {
  flex: 1;
  overflow-y: auto;
  padding: 1rem;
}

.cart-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 1rem 0;
  border-bottom: 1px solid #eee;
}

.cart-item:last-child {
  border-bottom: none;
}

.empty-cart {
  text-align: center;
  padding: 2rem;
  color: #666;
}

.cart-footer {
  padding: 1rem;
  border-top: 1px solid #eee;
  background-color: white;
}

.total {
  font-size: 1.2rem;
  font-weight: bold;
  margin-bottom: 1rem;
}

.checkout-button {
  width: 100%;
  padding: 0.75rem;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 1rem;
  cursor: pointer;
}

.checkout-button:disabled {
  background-color: #ccc;
  cursor: not-allowed;
}
</style> 