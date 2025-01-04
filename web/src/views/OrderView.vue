<template>
  <div class="order-container">
    <el-container>
      <el-header>
        <div class="header-content">
          <div class="logo">
            <h1>HungerYet</h1>
          </div>
          <div class="nav-actions">
            <el-button @click="goBack">返回</el-button>
          </div>
        </div>
      </el-header>

      <el-main>
        <div class="order-content">
          <el-tabs v-model="activeTab">
            <el-tab-pane label="全部订单" name="all">
              <el-table
                :data="orders"
                style="width: 100%"
                v-loading="loading"
              >
                <el-table-column prop="orderId" label="订单号" width="180" />
                <el-table-column prop="createTime" label="下单时间" width="180" />
                <el-table-column prop="totalAmount" label="总金额">
                  <template #default="scope">
                    ¥{{ scope.row.totalAmount }}
                  </template>
                </el-table-column>
                <el-table-column prop="status" label="状态">
                  <template #default="scope">
                    <el-tag :type="getStatusType(scope.row.status)">
                      {{ getStatusText(scope.row.status) }}
                    </el-tag>
                  </template>
                </el-table-column>
                <el-table-column label="操作">
                  <template #default="scope">
                    <el-button
                      size="small"
                      @click="viewOrderDetail(scope.row)"
                    >
                      查看详情
                    </el-button>
                    <el-button
                      v-if="scope.row.status === 'PENDING_PAYMENT'"
                      size="small"
                      type="primary"
                      @click="handlePayment(scope.row)"
                    >
                      去支付
                    </el-button>
                    <el-button
                      v-if="scope.row.status === 'COMPLETED'"
                      size="small"
                      type="success"
                      @click="handleComment(scope.row)"
                    >
                      评价
                    </el-button>
                  </template>
                </el-table-column>
              </el-table>

              <div class="pagination">
                <el-pagination
                  v-model:current-page="currentPage"
                  v-model:page-size="pageSize"
                  :total="total"
                  :page-sizes="[10, 20, 30, 50]"
                  layout="total, sizes, prev, pager, next, jumper"
                  @size-change="handleSizeChange"
                  @current-change="handleCurrentChange"
                />
              </div>
            </el-tab-pane>

            <el-tab-pane label="待支付" name="pending_payment">
              <!-- 待支付订单列表 -->
            </el-tab-pane>

            <el-tab-pane label="进行中" name="in_progress">
              <!-- 进行中订单列表 -->
            </el-tab-pane>

            <el-tab-pane label="已完成" name="completed">
              <!-- 已完成订单列表 -->
            </el-tab-pane>
          </el-tabs>
        </div>
      </el-main>
    </el-container>

    <!-- 订单详情抽屉 -->
    <el-drawer
      v-model="detailDrawer"
      title="订单详情"
      direction="rtl"
      size="500px"
    >
      <div v-if="currentOrder" class="order-detail">
        <div class="detail-header">
          <div class="order-id">订单号：{{ currentOrder.orderId }}</div>
          <div class="order-status">
            <el-tag :type="getStatusType(currentOrder.status)">
              {{ getStatusText(currentOrder.status) }}
            </el-tag>
          </div>
        </div>

        <div class="detail-content">
          <div class="section">
            <h3>订单信息</h3>
            <p>下单时间：{{ currentOrder.createTime }}</p>
            <p>支付方式：{{ currentOrder.paymentMethod || '-' }}</p>
            <p>备注信息：{{ currentOrder.remark || '无' }}</p>
          </div>

          <div class="section">
            <h3>商品信息</h3>
            <div class="items">
              <div v-for="item in currentOrder.items" :key="item.id" class="item">
                <span class="name">{{ item.name }}</span>
                <span class="quantity">x{{ item.quantity }}</span>
                <span class="price">¥{{ item.price }}</span>
              </div>
            </div>
            <div class="total">
              总计：<span class="amount">¥{{ currentOrder.totalAmount }}</span>
            </div>
          </div>

          <div class="section" v-if="currentOrder.status === 'COMPLETED'">
            <h3>评价信息</h3>
            <div v-if="currentOrder.comment">
              <el-rate v-model="currentOrder.comment.rating" disabled />
              <p class="comment-text">{{ currentOrder.comment.content }}</p>
            </div>
            <div v-else>
              <el-button type="primary" @click="handleComment(currentOrder)">
                去评价
              </el-button>
            </div>
          </div>
        </div>
      </div>
    </el-drawer>

    <!-- 评价对话框 -->
    <el-dialog
      v-model="commentDialog"
      title="订单评价"
      width="500px"
    >
      <el-form
        ref="commentForm"
        :model="commentForm"
        :rules="commentRules"
        label-width="80px"
      >
        <el-form-item label="评分" prop="rating">
          <el-rate v-model="commentForm.rating" />
        </el-form-item>
        <el-form-item label="评价内容" prop="content">
          <el-input
            v-model="commentForm.content"
            type="textarea"
            :rows="4"
            placeholder="请输入您的评价"
          />
        </el-form-item>
      </el-form>
      <template #footer>
        <span class="dialog-footer">
          <el-button @click="commentDialog = false">取消</el-button>
          <el-button type="primary" @click="submitComment" :loading="submitting">
            提交评价
          </el-button>
        </span>
      </template>
    </el-dialog>
  </div>
</template>

<script lang="ts" setup>
import { ref, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { ElMessage } from 'element-plus';
import HttpManager, { ReqId, Modules, ErrorCodes } from '../services/HttpManager';

const router = useRouter();
const httpManager = HttpManager.getInstance();

// 状态
const activeTab = ref('all');
const loading = ref(false);
const currentPage = ref(1);
const pageSize = ref(10);
const total = ref(0);
const orders = ref([]);
const detailDrawer = ref(false);
const currentOrder = ref(null);
const commentDialog = ref(false);
const submitting = ref(false);

const commentForm = ref({
  rating: 5,
  content: ''
});

const commentRules = {
  rating: [
    { required: true, message: '请选择评分', trigger: 'change' }
  ],
  content: [
    { required: true, message: '请输入评价内容', trigger: 'blur' },
    { min: 10, message: '评价内容不能少于10个字符', trigger: 'blur' }
  ]
};

// 方法
const getStatusType = (status: string) => {
  const types = {
    PENDING_PAYMENT: 'warning',
    PAID: 'primary',
    PREPARING: 'info',
    DELIVERING: '',
    COMPLETED: 'success',
    CANCELLED: 'danger'
  };
  return types[status] || 'info';
};

const getStatusText = (status: string) => {
  const texts = {
    PENDING_PAYMENT: '待支付',
    PAID: '已支付',
    PREPARING: '准备中',
    DELIVERING: '配送中',
    COMPLETED: '已完成',
    CANCELLED: '已取消'
  };
  return texts[status] || status;
};

const loadOrders = async () => {
  loading.value = true;
  try {
    const response = await new Promise((resolve) => {
      const handleResponse = (id: ReqId, res: string, err: ErrorCodes, mod: Modules) => {
        if (err === ErrorCodes.SUCCESS) {
          const response = JSON.parse(res);
          resolve(response);
        } else {
          ElMessage.error('网络错误，请稍后重试');
          resolve(null);
        }
        httpManager.off('httpFinished', handleResponse);
      };

      httpManager.on('httpFinished', handleResponse);
      httpManager.postHttpReq('/get_orders', {
        page: currentPage.value,
        pageSize: pageSize.value,
        status: activeTab.value === 'all' ? null : activeTab.value.toUpperCase()
      }, ReqId.ORDER, Modules.ORDERMOD);
    });

    if (response && response.code === 200) {
      orders.value = response.data.orders;
      total.value = response.data.total;
    }
  } finally {
    loading.value = false;
  }
};

const handleSizeChange = (val: number) => {
  pageSize.value = val;
  loadOrders();
};

const handleCurrentChange = (val: number) => {
  currentPage.value = val;
  loadOrders();
};

const viewOrderDetail = (order: any) => {
  currentOrder.value = order;
  detailDrawer.value = true;
};

const handlePayment = (order: any) => {
  // TODO: 实现支付功能
  ElMessage.info('支付功能开发中...');
};

const handleComment = (order: any) => {
  currentOrder.value = order;
  commentDialog.value = true;
  commentForm.value = {
    rating: 5,
    content: ''
  };
};

const submitComment = async () => {
  if (!currentOrder.value) return;
  
  submitting.value = true;
  try {
    const response = await new Promise((resolve) => {
      const handleResponse = (id: ReqId, res: string, err: ErrorCodes, mod: Modules) => {
        if (err === ErrorCodes.SUCCESS) {
          const response = JSON.parse(res);
          resolve(response);
        } else {
          ElMessage.error('网络错误，请稍后重试');
          resolve(null);
        }
        httpManager.off('httpFinished', handleResponse);
      };

      httpManager.on('httpFinished', handleResponse);
      httpManager.postHttpReq('/submit_comment', {
        orderId: currentOrder.value.orderId,
        rating: commentForm.value.rating,
        content: commentForm.value.content
      }, ReqId.ORDER, Modules.ORDERMOD);
    });

    if (response && response.code === 200) {
      ElMessage.success('评价提交成功');
      commentDialog.value = false;
      loadOrders();
    } else {
      ElMessage.error(response?.message || '评价提交失败');
    }
  } finally {
    submitting.value = false;
  }
};

const goBack = () => {
  router.push('/home');
};

// 生命周期
onMounted(() => {
  loadOrders();
});
</script>

<style lang="scss" scoped>
.order-container {
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
    }
  }

  .el-main {
    padding-top: 80px;
    
    .order-content {
      max-width: 1200px;
      margin: 0 auto;
      padding: 20px;
      background-color: #fff;
      border-radius: 4px;
      box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);

      .pagination {
        margin-top: 20px;
        text-align: right;
      }
    }
  }
}

.order-detail {
  padding: 20px;

  .detail-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
    padding-bottom: 20px;
    border-bottom: 1px solid #ebeef5;

    .order-id {
      font-size: 16px;
      color: #606266;
    }
  }

  .detail-content {
    .section {
      margin-bottom: 30px;

      h3 {
        margin: 0 0 15px;
        font-size: 16px;
        color: #303133;
      }

      p {
        margin: 8px 0;
        color: #606266;
      }

      .items {
        .item {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 10px 0;
          border-bottom: 1px solid #ebeef5;

          .name {
            flex: 1;
          }

          .quantity {
            margin: 0 20px;
            color: #909399;
          }

          .price {
            color: #f56c6c;
          }
        }
      }

      .total {
        margin-top: 15px;
        text-align: right;
        font-size: 16px;

        .amount {
          color: #f56c6c;
          font-weight: bold;
        }
      }

      .comment-text {
        margin-top: 10px;
        color: #606266;
      }
    }
  }
}
</style> 