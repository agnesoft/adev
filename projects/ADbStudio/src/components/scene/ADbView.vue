<template>
    <div class="adb-view" :class="{'search-mode': searchedWord !== ''}">
        <div class="view">
            <Edge v-for="(edges,key) in edgesGrouped" :key="key" :elements="edges" :viewSize="viewSize" :class="{selected: edgeIsSelected(edges)}" v-on="$listeners" />
            <Node v-for="node in nodes" :key="node.id" :element="node" :viewSize="viewSize" :class="{selected: node.id == searchedWord}" v-on="$listeners" />
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
        Edge,
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
            edgesGrouped: 'scene/getEdgesGrouped',
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
        },
        edgeIsSelected(edges){
            return edges.find((edge) => edge.id == this.searchedWord) ? true : false;
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
