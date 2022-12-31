import { createRouter, createWebHistory } from "vue-router"

const Views = {
	Home: () => import('@/views/Home.vue'),
	Networks: () => import('@/views/Networks.vue'),
	Connect: () => import('@/views/Connect.vue'),
	Finish: () => import('@/views/Finish.vue'),
}

const routes = [
	{ path: '/setup/', component: Views.Home },
	{ path: '/setup/networks', component: Views.Networks },
	{ path: '/setup/connect', component: Views.Connect },
	{ path: '/setup/finish', component: Views.Finish },
]

const router = createRouter({
	history: createWebHistory(),
	routes,
})

export default router;