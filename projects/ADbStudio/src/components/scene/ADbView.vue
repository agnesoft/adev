<template>
    <div class="adb-view" :class="{'search-mode': searchedWord !== ''}">
        <div class="view">
            <Edge v-for="edge in edges" :key="edge.id" :params="edge" :viewSize="viewSize" :class="{selected: edge.id == searchedWord}"/>
            <Node v-for="node in nodes" :key="node.id" :params="node" :viewSize="viewSize" :class="{selected: node.id == searchedWord}"/>
        </div>
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
            viewHeight: 0,
        }
    },
    props: {
        searchedWord: String,
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
        },
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
        window.removeEventListener('onmousewheel', this.onResize)
    }

}
</script>

<style scoped>
    .adb-view{
        position: relative;
        transform: scale(var(--scale,1)) translate(var(--move-x,0), var(--move-y,0));
        width: 100%;
        height: 100%;
    }
</style>
