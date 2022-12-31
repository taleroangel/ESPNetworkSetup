import { createApp } from 'vue'
import router from './router';
import axios from 'axios';
import VueAxios from 'vue-axios';

import './styles/styles.scss';
import App from './App.vue'

const app = createApp(App);
app.use(router);
app.use(VueAxios, axios);
app.provide("axios", app.config.globalProperties.axios);
app.mount('#app');
