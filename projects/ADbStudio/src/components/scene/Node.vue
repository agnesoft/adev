<template>
    <div class="node" :style="style">
        <div class="content">
            {{ params.id }}
        </div>
    </div>
</template>

<script>
export default {
    name: "Node",
    props: {
        params: Object,
        viewSize: Object
    },
    computed: {
        style: function(){
            return {
                '--node-size': this.node_size+"rem",
                top: this.top_pos, 
                left: this.left_pos
            }
        },
        top_pos: function(){
            let pos = this.params.coordinates.y*this.viewSize.height;
            return 'calc('+(pos).toString()+"px - "+(this.node_size/2).toString()+"rem)";
        },
        left_pos: function(){
            let pos = this.params.coordinates.x*this.viewSize.width;
            return 'calc('+(pos).toString()+"px - "+(this.node_size/2).toString()+"rem)";
        },
        node_size: function(){
            return "2.4";
        }
    },
}
</script>

<style scoped>
    .node{
        background-color: var(--primary-color);
        color: var(--dark-color);
        position: absolute;
        border-radius: 50%;
        width: var(--node-size);
        height: var(--node-size);
        line-height:  var(--node-size);
        text-align: center;
        cursor: pointer;
        transition: box-shadow var(--transition-ease);
        font-weight: 700;
    }
    
    .node:hover{
        box-shadow:  0 0 3px var(--primary-color), 2px 2px 5px var(--dark-color);
    }
</style>