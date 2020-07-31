<template>
    <div class="edge" :style="'--top_pos:'+ top_pos+'; --left_pos:'+ left_pos+'; --length:'+ length+'; --angle:'+ angle">
        <div class="content">
            {{ params.id }}
        </div>
    </div>
</template>

<script>
export default {
    name: "Edge",
    props: {
        params: Object,
        viewSize: Object
    },
    computed: {
        top_pos: function(){
            let y = (this.params.coordinates1.y + this.params.coordinates2.y) / 2 * this.viewSize.height;
            return (y).toString()+"px";
        },
        left_pos: function(){
            let x = (this.params.coordinates1.x + this.params.coordinates2.x) / 2 * this.viewSize.width - this.sqrt/2;
            return (x).toString()+"px";
        },
        length: function(){
            return (this.sqrt).toString()+"px";
        },
        angle: function(){
            let a = (this.params.coordinates1.x - this.params.coordinates2.x) * this.viewSize.width;
            let b = (this.params.coordinates1.y - this.params.coordinates2.y) * this.viewSize.height;
            return (Math.PI - Math.atan2(-b, a)).toString()+"rad";
        },
        sqrt: function(){
            let a = (this.params.coordinates1.x - this.params.coordinates2.x) * this.viewSize.width;
            let b = (this.params.coordinates1.y - this.params.coordinates2.y) * this.viewSize.height;
            return Math.sqrt(a * a + b * b);
        } 
    },
}
</script>

<style scoped>
    .edge{
        border: 2px solid var(--secondary-color);
        width:  var(--length); 
        text-align: center;
        height: 0px; 
        transform: rotate(var(--angle));
        position: absolute; 
        top: var(--top_pos); 
        left: var(--left_pos);
        cursor: pointer;
        transition: box-shadow var(--transition-ease); 
    }
    .edge:hover{
        box-shadow:  0 0 3px var(--primary-color), 2px 2px 5px var(--dark-color);
    }
</style>