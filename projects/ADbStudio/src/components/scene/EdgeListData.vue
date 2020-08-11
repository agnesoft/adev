<template>   
    <ListData :data="edges" v-on="$listeners"/>
</template>

<script>
import { mapGetters } from 'vuex'
import ListData from './ListData';

export default {
    name: "EdgeListData",
    components: {
        ListData,
    },
    computed: {
        ...mapGetters({
            edges: 'scene/getEdges',
        }),
    },
}
</script>

<style scoped>

</style>
