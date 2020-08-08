<template>
    <ListData :data="nodes" />
</template>

<script>
import { mapGetters } from 'vuex'
import ListData from './ListData';

export default {
    name: "NodeListData",
    components: {
        ListData,
    },
    computed: {
        ...mapGetters({
            nodes: 'scene/getNodes',
        }),
    },
}
</script>

<style scoped>

</style>
