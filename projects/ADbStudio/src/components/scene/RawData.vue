<template>
  <div class="raw-data">
      <pre>{{ data }}</pre>
  </div>
</template>

<script>
import { mapGetters } from 'vuex'

export default {
    name: "RawData",
    computed: {
        ...mapGetters({
            data: 'scene/getData',
        }),
    },
}
</script>

<style scoped>

</style>
