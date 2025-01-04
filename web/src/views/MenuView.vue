<template>
  <div class="menu-container">
    <el-container>
      <el-header>
        <div class="header-content">
          <div class="logo">
            <h1>HungerYet</h1>
          </div>
          <div class="nav-actions">
            <el-button type="primary" @click="viewCart" :badge="cartCount">
              <el-icon><shopping-cart /></el-icon>
              购物车
            </el-button>
            <el-button @click="goBack">返回</el-button>
          </div>
        </div>
      </el-header>

      <el-main>
        <div class="menu-content">
          <el-row :gutter="20">
            <el-col :span="6">
              <div class="category-list">
                <el-menu
                  :default-active="activeCategory"
                  @select="handleCategorySelect"
                >
                  <el-menu-item 
                    v-for="category in categories" 
                    :key="category.id" 
                    :index="category.id"
                  >
                    {{ category.name }}
                  </el-menu-item>
                </el-menu>
              </div>
            </el-col>

            <el-col :span="18">
              <div class="dish-list">
                <el-row :gutter="20">
                  <el-col 
                    v-for="dish in currentDishes" 
                    :key="dish.id" 
                    :span="8"
                  >
                    <el-card class="dish-card">
                      <img :src="dish.image" class="dish-image" />
                      <div class="dish-info">
                        <h3>{{ dish.name }}</h3>
                        <p class="description">{{ dish.description }}</p>
                        <div class="price-action">
                          <span class="price">¥{{ dish.price }}</span>
                          <el-button-group>
                            <el-button 
                              type="primary" 
                              circle
                              @click="decreaseQuantity(dish)"
                              :disabled="!getCartQuantity(dish.id)"
                            >
                              <el-icon><minus /></el-icon>
                            </el-button>
                            <span class="quantity">{{ getCartQuantity(dish.id) }}</span>
                            <el-button 
                              type="primary" 
                              circle
                              @click="increaseQuantity(dish)"
                            >
                              <el-icon><plus /></el-icon>
                            </el-button>
                          </el-button-group>
                        </div>
                      </div>
                    </el-card>
                  </el-col>
                </el-row>
              </div>
            </el-col>
          </el-row>
        </div>
      </el-main>
    </el-container>

    <!-- 购物车抽屉 -->
    <el-drawer
      v-model="cartDrawer"
      title="购物车"
      direction="rtl"
      size="400px"
    >
      <div class="cart-content">
        <div v-if="cartItems.length === 0" class="empty-cart">
          <el-empty description="购物车是空的" />
        </div>
        <template v-else>
          <div class="cart-items">
            <div v-for="item in cartItems" :key="item.id" class="cart-item">
              <div class="item-info">
                <h4>{{ item.name }}</h4>
                <span class="price">¥{{ item.price }}</span>
              </div>
              <div class="item-action">
                <el-button-group>
                  <el-button 
                    type="primary" 
                    circle
                    @click="decreaseQuantity(item)"
                  >
                    <el-icon><minus /></el-icon>
                  </el-button>
                  <span class="quantity">{{ item.quantity }}</span>
                  <el-button 
                    type="primary" 
                    circle
                    @click="increaseQuantity(item)"
                  >
                    <el-icon><plus /></el-icon>
                  </el-button>
                </el-button-group>
              </div>
            </div>
          </div>
          <div class="cart-footer">
            <div class="total">
              总计: <span class="price">¥{{ totalPrice }}</span>
            </div>
            <el-button type="primary" @click="submitOrder" :loading="submitting">
              提交订单
            </el-button>
          </div>
        </template>
      </div>
    </el-drawer>
  </div>
</template>

<script lang="ts" setup>
import { ref, computed } from 'vue';
import { useRouter } from 'vue-router';
import { ElMessage } from 'element-plus';
import { ShoppingCart, Plus, Minus } from '@element-plus/icons-vue';
import HttpManager, { ReqId, Modules, ErrorCodes } from '../services/HttpManager';
import { ApiResponse } from '../types/api';

interface Dish {
  id: string;
  name: string;
  description: string;
  price: number;
  image: string;
  categoryId: string;
}

interface Category {
  id: string;
  name: string;
}

interface CartItem extends Dish {
  quantity: number;
}

interface CreateOrderResponse {
  orderId: string;
  code: number;
  message: string;
}

const router = useRouter();
const httpManager = HttpManager.getInstance();

// 状态
const activeCategory = ref('1');
const cartDrawer = ref(false);
const submitting = ref(false);
const categories = ref<Category[]>([
  { id: '1', name: '热销推荐' },
  { id: '2', name: '主食' },
  { id: '3', name: '小吃' },
  { id: '4', name: '饮品' },
]);
const currentDishes = ref<Dish[]>([
  {
    id: '1',
    name: '红烧牛肉面',
    description: '精选牛肉，传统工艺',
    price: 28,
    image: '/images/beef-noodles.jpg',
    categoryId: '1'
  },
  // 更多菜品...
]);
const cart = ref<Map<string, CartItem>>(new Map());

// 计算属性
const cartItems = computed(() => {
  return Array.from(cart.value.values());
});

const cartCount = computed(() => {
  return cartItems.value.reduce((sum, item) => sum + item.quantity, 0);
});

const totalPrice = computed(() => {
  return cartItems.value.reduce((sum, item) => sum + item.price * item.quantity, 0);
});

// 方法
const handleCategorySelect = (categoryId: string) => {
  activeCategory.value = categoryId;
  // TODO: 根据分类加载菜品
};

const getCartQuantity = (dishId: string) => {
  return cart.value.get(dishId)?.quantity || 0;
};

const increaseQuantity = (dish: any) => {
  if (cart.value.has(dish.id)) {
    const item = cart.value.get(dish.id);
    item.quantity++;
  } else {
    cart.value.set(dish.id, {
      ...dish,
      quantity: 1
    });
  }
  cart.value = new Map(cart.value);
};

const decreaseQuantity = (dish: any) => {
  if (cart.value.has(dish.id)) {
    const item = cart.value.get(dish.id);
    item.quantity--;
    if (item.quantity <= 0) {
      cart.value.delete(dish.id);
    }
    cart.value = new Map(cart.value);
  }
};

const viewCart = () => {
  cartDrawer.value = true;
};

const submitOrder = async () => {
  if (cartItems.value.length === 0) {
    ElMessage.warning('请先选择商品');
    return;
  }

  submitting.value = true;
  const orderItems = cartItems.value.map(item => ({
    dishId: item.id,
    quantity: item.quantity
  }));

  try {
    const response = await new Promise<ApiResponse<CreateOrderResponse>>((resolve, reject) => {
      const handleResponse = (id: ReqId, res: string, err: ErrorCodes, mod: Modules) => {
        if (err === ErrorCodes.SUCCESS) {
          const response = JSON.parse(res) as ApiResponse<CreateOrderResponse>;
          resolve(response);
        } else {
          reject(err);
        }
        httpManager.off('httpFinished', handleResponse);
      };

      httpManager.on('httpFinished', handleResponse);
      httpManager.postHttpReq<CreateOrderResponse>('/create_order', { items: orderItems }, ReqId.ORDER, Modules.ORDERMOD);
    });

    if (response.code === 200) {
      ElMessage.success('订单提交成功');
      cart.value.clear();
      cartDrawer.value = false;
      router.push('/order');
    } else {
      ElMessage.error(response.message || '订单提交失败');
    }
  } catch (error) {
    ElMessage.error('网络错误，请稍后重试');
  } finally {
    submitting.value = false;
  }
};

const goBack = () => {
  router.push('/home');
};
</script>

<style lang="scss" scoped>
.menu-container {
  min-height: 100vh;
  background-color: #f5f7fa;

  .el-container {
    min-height: 100vh;
  }

  .el-header {
    background-color: #fff;
    box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
    position: fixed;
    width: 100%;
    z-index: 100;
    padding: 0;

    .header-content {
      max-width: 1200px;
      margin: 0 auto;
      height: 60px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 0 20px;

      .logo {
        h1 {
          margin: 0;
          color: #409EFF;
        }
      }

      .nav-actions {
        display: flex;
        gap: 10px;
      }
    }
  }

  .el-main {
    padding-top: 80px;
    
    .menu-content {
      max-width: 1200px;
      margin: 0 auto;
      padding: 20px;

      .category-list {
        background-color: #fff;
        border-radius: 4px;
        box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
      }

      .dish-list {
        .dish-card {
          margin-bottom: 20px;
          
          .dish-image {
            width: 100%;
            height: 200px;
            object-fit: cover;
          }

          .dish-info {
            padding: 14px;

            h3 {
              margin: 0 0 10px;
              font-size: 16px;
            }

            .description {
              color: #909399;
              font-size: 14px;
              margin-bottom: 10px;
              height: 40px;
              overflow: hidden;
            }

            .price-action {
              display: flex;
              justify-content: space-between;
              align-items: center;

              .price {
                color: #f56c6c;
                font-size: 20px;
                font-weight: bold;
              }

              .quantity {
                display: inline-block;
                width: 30px;
                text-align: center;
                font-size: 16px;
              }
            }
          }
        }
      }
    }
  }
}

.cart-content {
  height: 100%;
  display: flex;
  flex-direction: column;

  .empty-cart {
    flex: 1;
    display: flex;
    justify-content: center;
    align-items: center;
  }

  .cart-items {
    flex: 1;
    overflow-y: auto;
    padding: 20px;

    .cart-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 10px 0;
      border-bottom: 1px solid #ebeef5;

      .item-info {
        h4 {
          margin: 0 0 5px;
        }

        .price {
          color: #f56c6c;
        }
      }

      .item-action {
        .quantity {
          display: inline-block;
          width: 30px;
          text-align: center;
          font-size: 16px;
        }
      }
    }
  }

  .cart-footer {
    padding: 20px;
    border-top: 1px solid #ebeef5;
    background-color: #fff;

    .total {
      margin-bottom: 20px;
      font-size: 18px;
      
      .price {
        color: #f56c6c;
        font-weight: bold;
      }
    }

    .el-button {
      width: 100%;
    }
  }
}
</style> 