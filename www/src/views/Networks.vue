<template>
	<h2>Select a network to connect to:</h2>

	<!-- List of networks -->
	<h1 v-if="!state.networkList">Fetching network list...</h1>
	<div class="list" v-if="state.networkList">
		<span v-for="network in state.networkList" :key="network" class="item" @click="onNetworkSelect">
			{{ network }}</span>
	</div>

	<a class="btn" v-if="state.currentNetwork != null"
		:href="`/setup/connect?ssid=${state.currentNetwork.innerText}`">Connect</a>
</template>

<script setup>
/* --------- Imports --------- */
import { reactive, onMounted, inject } from 'vue';
const axios = inject('axios');

/* --------- Variables --------- */
const state = reactive({
	currentNetwork: null,
	networkList: null
});

/* --------- Functions --------- */
function onNetworkSelect(event) {
	// Get current node
	const node = event.target;

	// Remove last node
	if (state.currentNetwork != null)
		state.currentNetwork.classList.remove('active');

	// Add new node
	node.classList.add('active');
	state.currentNetwork = node;
}

function fetchNetworkList() {
	axios.get('/setup/api/list', {})
		.then(e => {
			if (e.data == null) throw "Server returned null";
			state.networkList = new Set(e.data);
		})
		.catch((e) => { console.error(e); });
}

/* --------- Hooks --------- */
onMounted(() => {
	fetchNetworkList();
	setInterval(fetchNetworkList, 10000);
});

</script>