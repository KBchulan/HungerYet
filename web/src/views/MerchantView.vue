<template>
  <div class="merchant-container">
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
        <div class="merchant-content">
          <el-card class="merchant-info">
            <template #header>
              <div class="merchant-header">
                <div class="merchant-basic">
                  <img :src="merchantInfo.logo" class="merchant-logo" />
                  <div class="merchant-text">
                    <h2>{{ merchantInfo.name }}</h2>
                    <div class="rating">
                      <el-rate
                        v-model="merchantInfo.rating"
                        disabled
                        show-score
                        text-color="#ff9900"
                      />
                      <span class="order-count">月售 {{ merchantInfo.monthlyOrders }}</span>
                    </div>
                  </div>
                </div>
                <div class="merchant-status">
                  <el-tag :type="merchantInfo.status === 'OPEN' ? 'success' : 'info'">
                    {{ merchantInfo.status === 'OPEN' ? '营业中' : '休息中' }}
                  </el-tag>
                </div>
              </div>
            </template>

            <div class="merchant-detail">
              <div class="info-item">
                <i class="el-icon-location"></i>
                <span>{{ merchantInfo.address }}</span>
              </div>
              <div class="info-item">
                <i class="el-icon-time"></i>
                <span>营业时间：{{ merchantInfo.businessHours }}</span>
              </div>
              <div class="info-item">
                <i class="el-icon-phone"></i>
                <span>联系电话：{{ merchantInfo.phone }}</span>
              </div>
              <div class="info-item">
                <i class="el-icon-info"></i>
                <span>{{ merchantInfo.description }}</span>
              </div>
            </div>
          </el-card>

          <el-card class="merchant-comments">
            <template #header>
              <div class="comments-header">
                <h3>顾客评价</h3>
                <div class="filter">
                  <el-radio-group v-model="commentFilter" size="small">
                    <el-radio-button label="all">全部</el-radio-button>
                    <el-radio-button label="good">好评</el-radio-button>
                    <el-radio-button label="medium">中评</el-radio-button>
                    <el-radio-button label="bad">差评</el-radio-button>
                  </el-radio-group>
                </div>
              </div>
            </template>

            <div class="comments-list" v-loading="loading">
              <div v-for="comment in filteredComments" :key="comment.id" class="comment-item">
                <div class="comment-header">
                  <div class="user-info">
                    <img :src="comment.userAvatar" class="user-avatar" />
                    <span class="username">{{ comment.username }}</span>
                  </div>
                  <div class="comment-rating">
                    <el-rate v-model="comment.rating" disabled />
                    <span class="comment-time">{{ comment.createTime }}</span>
                  </div>
                </div>
                <div class="comment-content">
                  {{ comment.content }}
                </div>
                <div class="order-info" v-if="comment.orderItems">
                  <span class="label">订单内容：</span>
                  <span class="items">{{ comment.orderItems.join('、') }}</span>
                </div>
                <div class="merchant-reply" v-if="comment.merchantReply">
                  <span class="label">商家回复：</span>
                  <span class="reply">{{ comment.merchantReply }}</span>
                </div>
              </div>

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
            </div>
          </el-card>
        </div>
      </el-main>
    </el-container>
  </div>
</template>

<script lang="ts" setup>
import { ref, computed, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { ElMessage } from 'element-plus';
import HttpManager, { ReqId, Modules, ErrorCodes } from '../services/HttpManager';

const router = useRouter();
const httpManager = HttpManager.getInstance();

// 状态
const loading = ref(false);
const currentPage = ref(1);
const pageSize = ref(10);
const total = ref(0);
const commentFilter = ref('all');

interface MerchantInfo {
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

interface Comment {
  id: string;
  username: string;
  userAvatar: string;
  rating: number;
  content: string;
  createTime: string;
  orderItems?: string[];
  merchantReply?: string;
}

const merchantInfo = ref<MerchantInfo>({
  id: '',
  name: '',
  logo: '',
  rating: 0,
  monthlyOrders: 0,
  status: 'CLOSED',
  address: '',
  businessHours: '',
  phone: '',
  description: ''
});

const comments = ref<Comment[]>([]);

// 计算属性
const filteredComments = computed(() => {
  if (commentFilter.value === 'all') return comments.value;
  
  const ratingRanges = {
    good: [4, 5],
    medium: [2, 3],
    bad: [0, 1]
  };
  
  const range = ratingRanges[commentFilter.value as keyof typeof ratingRanges];
  return comments.value.filter(comment => 
    comment.rating >= range[0] && comment.rating <= range[1]
  );
});

// 方法
const loadMerchantInfo = async () => {
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
      httpManager.postHttpReq('/get_merchant_info', {}, ReqId.MERCHANT, Modules.MERCHANTMOD);
    });

    if (response && response.code === 200) {
      merchantInfo.value = response.data;
    }
  } catch (error) {
    ElMessage.error('获取商家信息失败');
  }
};

const loadComments = async () => {
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
      httpManager.postHttpReq('/get_merchant_comments', {
        page: currentPage.value,
        pageSize: pageSize.value
      }, ReqId.MERCHANT, Modules.MERCHANTMOD);
    });

    if (response && response.code === 200) {
      comments.value = response.data.comments;
      total.value = response.data.total;
    }
  } finally {
    loading.value = false;
  }
};

const handleSizeChange = (val: number) => {
  pageSize.value = val;
  loadComments();
};

const handleCurrentChange = (val: number) => {
  currentPage.value = val;
  loadComments();
};

const goBack = () => {
  router.push('/home');
};

// 生命周期
onMounted(() => {
  loadMerchantInfo();
  loadComments();
});
</script>

<style lang="scss" scoped>
.merchant-container {
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
    
    .merchant-content {
      max-width: 1200px;
      margin: 0 auto;
      padding: 20px;

      .merchant-info {
        margin-bottom: 20px;

        .merchant-header {
          display: flex;
          justify-content: space-between;
          align-items: center;

          .merchant-basic {
            display: flex;
            align-items: center;

            .merchant-logo {
              width: 80px;
              height: 80px;
              border-radius: 4px;
              margin-right: 20px;
            }

            .merchant-text {
              h2 {
                margin: 0 0 10px;
              }

              .rating {
                display: flex;
                align-items: center;

                .order-count {
                  margin-left: 10px;
                  color: #909399;
                }
              }
            }
          }
        }

        .merchant-detail {
          margin-top: 20px;

          .info-item {
            margin-bottom: 10px;
            color: #606266;

            i {
              margin-right: 8px;
            }
          }
        }
      }

      .merchant-comments {
        .comments-header {
          display: flex;
          justify-content: space-between;
          align-items: center;

          h3 {
            margin: 0;
          }
        }

        .comments-list {
          .comment-item {
            padding: 20px 0;
            border-bottom: 1px solid #ebeef5;

            &:last-child {
              border-bottom: none;
            }

            .comment-header {
              display: flex;
              justify-content: space-between;
              align-items: center;
              margin-bottom: 10px;

              .user-info {
                display: flex;
                align-items: center;

                .user-avatar {
                  width: 40px;
                  height: 40px;
                  border-radius: 50%;
                  margin-right: 10px;
                }

                .username {
                  color: #303133;
                }
              }

              .comment-rating {
                display: flex;
                align-items: center;

                .comment-time {
                  margin-left: 10px;
                  color: #909399;
                  font-size: 14px;
                }
              }
            }

            .comment-content {
              margin: 10px 0;
              color: #303133;
            }

            .order-info,
            .merchant-reply {
              color: #606266;
              font-size: 14px;
              margin-top: 10px;

              .label {
                color: #909399;
              }
            }

            .merchant-reply {
              background-color: #f5f7fa;
              padding: 10px;
              border-radius: 4px;
              margin-top: 10px;
            }
          }

          .pagination {
            margin-top: 20px;
            text-align: right;
          }
        }
      }
    }
  }
}
</style> 