import { defineStore } from 'pinia';
import HttpManager, { ReqId, Modules, ErrorCodes } from '../services/HttpManager';

interface UserState {
  token: string | null;
  isAdmin: boolean;
  userId: string | null;
  username: string | null;
  email: string | null;
}

export const useUserStore = defineStore('user', {
  state: (): UserState => ({
    token: localStorage.getItem('token'),
    isAdmin: localStorage.getItem('isAdmin') === 'true',
    userId: localStorage.getItem('userId'),
    username: localStorage.getItem('username'),
    email: localStorage.getItem('email'),
  }),

  getters: {
    isLoggedIn: (state) => !!state.token,
  },

  actions: {
    async login(email: string, password: string): Promise<boolean> {
      try {
        const httpManager = HttpManager.getInstance();
        
        return new Promise((resolve) => {
          const handleResponse = (id: ReqId, res: string, err: ErrorCodes, mod: Modules) => {
            console.log('Login response received:', { id, res, err, mod });
            
            if (err === ErrorCodes.SUCCESS && mod === Modules.LOGINMOD) {
              try {
                const response = JSON.parse(res);
                console.log('Parsed response:', response);
                
                if (response.error === 0) {
                  this.setUserInfo({
                    token: response.token,
                    isAdmin: false,
                    userId: response.uid,
                    username: response.name,
                    email: response.email
                  });
                  resolve(true);
                } else {
                  console.log('Login failed with error:', response.error);
                  resolve(false);
                }
              } catch (e) {
                console.error('Error parsing response:', e);
                resolve(false);
              }
              httpManager.off('httpFinished', handleResponse);
            } else if (err === ErrorCodes.ERR_NETWORK) {
              console.log('Network error during login');
              resolve(false);
              httpManager.off('httpFinished', handleResponse);
            }
          };

          httpManager.on('httpFinished', handleResponse);
          const encryptedPassword = this.xorString(password);
          console.log('Sending login request with:', { email, encryptedPassword });
          httpManager.postHttpReq('/user_login', { 
            email, 
            passwd: encryptedPassword
          }, ReqId.LOGIN, Modules.LOGINMOD);
        });
      } catch (error) {
        console.error('Login error:', error);
        return false;
      }
    },

    setUserInfo(data: any) {
      this.token = data.token;
      this.isAdmin = data.isAdmin;
      this.userId = data.userId;
      this.username = data.username;
      this.email = data.email;

      localStorage.setItem('token', data.token);
      localStorage.setItem('isAdmin', data.isAdmin.toString());
      localStorage.setItem('userId', data.userId);
      localStorage.setItem('username', data.username);
      localStorage.setItem('email', data.email);
    },

    logout() {
      this.token = null;
      this.isAdmin = false;
      this.userId = null;
      this.username = null;
      this.email = null;

      localStorage.removeItem('token');
      localStorage.removeItem('isAdmin');
      localStorage.removeItem('userId');
      localStorage.removeItem('username');
      localStorage.removeItem('email');
    },

    xorString(str: string): string {
      const key = 'WhxChat';
      let result = '';
      
      for (let i = 0; i < str.length; i++) {
        result += String.fromCharCode(str.charCodeAt(i) ^ key.charCodeAt(i % key.length));
      }
      
      return result;
    }
  },
}); 