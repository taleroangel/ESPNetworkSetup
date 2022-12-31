<template>
	<h2>🚀 Setup Finished</h2>
	<p>Click on 'Save and Reboot' to finish the setup and reboot the device</p>
	<p>Once rebooted you can close this page, to setup your device once again use the
		<code>NetworkSetup::reset();</code> function
	</p>
	<a href="#" class="btn" @click="finish_reboot">Save and Reboot</a>
</template>

<script setup>
import { inject } from 'vue';
const axios = inject('axios');
function finish_reboot() {
	axios.post('/setup/api/done');
}
</script>

<style lang="scss" scoped>

</style>