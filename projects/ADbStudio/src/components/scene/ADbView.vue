<template>
    <div class="adb-view">
        <Edge v-for="edge in edges" :key="edge.id" :params="edge" :viewSize="viewSize"/>
        <Node v-for="node in nodes" :key="node.id" :params="node" :viewSize="viewSize"/>
    </div>
</template>

<script>
import Node from "./Node";
import Edge from "./Edge";
import { mapGetters } from 'vuex'

export default {
    name: "ADbView",
    components: {
        Node,
        Edge
    },
    data(){
        return {
            viewWidth: 0,
            viewHeight: 0
        }
    },
    computed: {
        ...mapGetters({
            nodes: 'scene/getNodes',
            edges: 'scene/getEdges',
        }),
        viewSize(){
            return {
                width: this.viewWidth,
                height: this.viewHeight
            }
        }
    },
    methods: {
        onResize() {
            this.viewWidth = document.documentElement.clientWidth;
            this.viewHeight = document.documentElement.clientHeight;
        }
    },
    mounted() {
        window.addEventListener('resize', this.onResize)
        this.onResize();
    },

    beforeDestroy() {
        window.removeEventListener('resize', this.onResize)
    }

}
</script>

<style scoped>
    .adb-view{
        position: relative;
    }
</style>
