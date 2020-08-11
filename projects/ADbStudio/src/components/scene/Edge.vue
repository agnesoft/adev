<template>
    <div class="edge" :style="'--top_pos:'+ top_pos+'; --left_pos:'+ left_pos+'; --length:'+ length+'; --angle:'+ angle">
        <Element :element="element" v-on="$listeners" />
    </div>
</template>

<script>
import Element from "./Element"

export default {
    name: "Edge",
    components: {
        Element
    },
    props: {
        element: Object,
        viewSize: Object
    },
    computed: {
        top_pos: function(){
            let y = (this.element.coordinates1.y + this.element.coordinates2.y) / 2 * this.viewSize.height;
            return (y).toString()+"px";
        },
        left_pos: function(){
            let x = (this.element.coordinates1.x + this.element.coordinates2.x) / 2 * this.viewSize.width - this.sqrt/2;
            return (x).toString()+"px";
        },
        length: function(){
            return (this.sqrt).toString()+"px";
        },
        angle: function(){
            let a = (this.element.coordinates1.x - this.element.coordinates2.x) * this.viewSize.width;
            let b = (this.element.coordinates1.y - this.element.coordinates2.y) * this.viewSize.height;
            return (Math.PI - Math.atan2(-b, a)).toString()+"rad";
        },
        sqrt: function(){
            let a = (this.element.coordinates1.x - this.element.coordinates2.x) * this.viewSize.width;
            let b = (this.element.coordinates1.y - this.element.coordinates2.y) * this.viewSize.height;
            return Math.sqrt(a * a + b * b);
        } 
    }
}
</script>

<style scoped>
    .edge{
        height: 4px;
        background-color: var(--secondary-color);
        /* border: 2px solid var(--secondary-color); */
        color: var(--dark-color);
        width:  var(--length); 
        text-align: center;
        transform: rotate(var(--angle));
        position: absolute; 
        top: var(--top_pos); 
        left: var(--left_pos);
        cursor: pointer;
        transition: all var(--transition-ease); 
    }
    .content{
        position: relative;
    }
    .edge:hover{
        box-shadow:  0 0 3px var(--primary-color), 2px 2px 5px var(--dark-color);
    }
    .search-mode .edge{
        background-color: #f2ede8;
        color: #c4b1a1;
    }
    .edge.selected{
        background-color: var(--secondary-color);
        color: var(--dark-color);
    }
    .element{
        position: relative;
        top: 4px;
    }
    /* .element-data{
        transform: rotate(calc(-1 * var(--angle)));
    } */
</style>
