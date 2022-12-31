<template>
	<h2>📡 Connect to Network</h2>
	<div v-if="state.error" class="error">
		<h3>⚠️ Error: Unable to connect to network</h3>
		<p>{{ state.error }}</p>
	</div>
	<div v-if="state.attempt" class="warning">
		<h3>🔐 Attempting connection</h3>
		<p>Wait until device is connected to network.<br />
			<b>Current number of attemps: {{ state.attempt }} </b> <br />
			<i>Connection Status: {{ connection_status[state.conn_status] }}</i>
		</p>
	</div>
	<div v-if="state.success" class="success">
		<h3>🚀 Successful connection</h3>
		<p>Click on next to continue the procedure</p>
	</div>
	<div class="list">
		<h4 class="label">Network SSID:</h4>
		<h3 class="item">{{ route['ssid'] }}</h3>
		<p>If the network has no password leave this field empty</p>
		<label class="label" for="password_input">Password:</label>
		<input type="password" autocomplete="false" name="password_input" v-model="password_input" />
		<button type="submit" class="btn" @click="init_connection"
			:disabled="(state.attempt != null) || (state.success == true)">Connect</button>
	</div>
	<a v-if="state.success" href="/setup/next" class="btn">Continue Setup</a>
</template>

<script setup>

/* --------- Imports --------- */
import { inject, ref, reactive } from 'vue';
import { useRoute } from 'vue-router';

/* --------- Variables --------- */
const route = useRoute().query;
const axios = inject('axios');

const state = reactive({
	error: null, /* An error ocurred, error message goes here */
	attempt: null, /* Number of connection API call requests */
	conn_status: "WL_NO_SSID_AVAIL",
	success: false,
});

const password_input = ref();
const max_attempts = 10;
let interval = null;

const connection_status = [
	"WL_IDLE_STATUS", //0
	"WL_NO_SSID_AVAIL", //1
	"WL_SCAN_COMPLETED", //2
	"WL_CONNECTED", //3
	"WL_CONNECT_FAILED", //4
	"WL_CONNECTION_LOST", //5
	"WL_WRONG_PASSWORD", //6
	"WL_DISCONNECTE", //7
];

/* --------- Functions --------- */
function check_connection() {
	axios.get('/setup/api/status', {})
		.then(i => {
			state.conn_status = i.data;

			// Check errors
			if (state.conn_status == 3) {
				clearInterval(interval);
				state.attempt = null;
				state.error = null;
				state.success = true;
				return;
			}

			else if (state.conn_status != 7) {
				clearInterval(interval);
				state.attempt = null;
				state.error = "Unsuccessful connection, try another password";
				return;
			}

			state.attempt++; // Mark attempt
			// If attempts exceed maximum, stop
			if (state.attempt >= max_attempts) {
				clearInterval(interval);
				state.attempt = null;
				state.error = "Connection attempt timed out";
			}
		})
		.catch(e => {
			state.error = e;
		})
}

function init_connection() {
	axios.post(`/setup/api/connect?ssid=${route['ssid']}&password=${password_input.value}`, {})
		.then(_ => {
			state.error = null;
			state.attempt = 1;
			interval = setInterval(check_connection, 5000);
		})
		.catch(e => {
			state.error = e;
		})
}
</script>