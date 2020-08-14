<template>
    <div class="edge" :style="'--top_pos:'+ top_pos+'; --left_pos:'+ left_pos+'; --length:'+ length+'; --angle:'+ angle">
        <div class="arrow back" v-if="direction <= 0"></div>
        <div class="arrow front" v-if="direction >= 0"></div>
        <Element v-if="elements.length === 1" :element="elements[0]" v-on="$listeners" />
        <Elements v-else :elements="elements" v-on="$listeners" />
    </div>
</template>

<script>
import Element from "./Element"
import Elements from "./Elements"

export default {
    name: "Edge",
    components: {
        Element,
        Elements
    },
    props: {
        elements: Array,
        viewSize: Object
    },
    computed: {
        coordinates: function(){
            let coord;
            if(this.elements[0].coordinates1.x < this.elements[0].coordinates2.x){
                coord = {
                    c1: this.elements[0].coordinates1,
                    c2: this.elements[0].coordinates2,
                }
            } else if(this.elements[0].coordinates1.x > this.elements[0].coordinates2.x) {
                coord = {
                    c1: this.elements[0].coordinates2,
                    c2: this.elements[0].coordinates1,
                }
            } else {
                if(this.elements[0].coordinates1.y < this.elements[0].coordinates2.y){
                    coord = {
                        c1: this.elements[0].coordinates2,
                        c2: this.elements[0].coordinates1,
                    }
                } else {
                    coord = {
                        c1: this.elements[0].coordinates1,
                        c2: this.elements[0].coordinates2,
                    }
                }
            }
            return coord;
        },
        direction: function(){
            let arrow = 0;
            if(this.elements[0].coordinates1.x < this.elements[0].coordinates2.x){
                arrow = 1;
            } else if(this.elements[0].coordinates1.x > this.elements[0].coordinates2.x) {
                arrow = -1;
            } else {
                if(this.elements[0].coordinates1.y < this.elements[0].coordinates2.y){
                    arrow = -1;
                } else {
                    arrow = 1;
                }
            }
            if(this.elements.find((element) => element.nodes[0] !== this.elements[0].nodes[0])){
                arrow = 0
            }
            return arrow;
        },
        top_pos: function(){
            let y = (this.coordinates.c1.y + this.coordinates.c2.y) / 2 * this.viewSize.height;
            return (y).toString()+"px";
        },
        left_pos: function(){
            let x = (this.coordinates.c1.x + this.coordinates.c2.x) / 2 * this.viewSize.width - this.sqrt/2;
            return (x).toString()+"px";
        },
        length: function(){
            return (this.sqrt).toString()+"px";
        },
        angle: function(){
            let a = (this.coordinates.c1.x - this.coordinates.c2.x) * this.viewSize.width;
            let b = (this.coordinates.c1.y - this.coordinates.c2.y) * this.viewSize.height;
            return (Math.PI - Math.atan2(-b, a)).toString()+"rad";
        },
        sqrt: function(){
            let a = (this.coordinates.c1.x - this.coordinates.c2.x) * this.viewSize.width;
            let b = (this.coordinates.c1.y - this.coordinates.c2.y) * this.viewSize.height;
            return Math.sqrt(a * a + b * b);
        },
    }
}
</script>

<style scoped>
    .edge{
        --width: 0.3rem;
        height: var(--width);
        top: calc(-0.5 * var(--width));
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
        --size: 0.8rem;
    }
    
    .edge:before,.edge:after{
        content: "";
        display: block;
        width: var(--size);
        height: var(--size);
        background-color: var(--secondary-color);
        border-radius: 50%;
        position: absolute;
        top: calc(0.5 * (var(--width) - var(--size)));
        transition: all var(--transition-ease); 
    }
    .edge:before{
        left: calc(-0.5 * var(--size));
    }
    .edge:after{
        left: calc(-0.5 * var(--size) + var(--length));
    }
    .content{
        position: relative;
    }
    .edge:hover,.edge:hover::before,.edge:hover::after{
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
        top: -1rem;
        width: 100%;
        height: 2rem;
    }
    .arrow{
        --arrow-size: 0.5rem;
        border: solid var(--dark-color);
        border-width: 0 3px 3px 0;
        display: inline-block;
        padding: var(--arrow-size);
        position: absolute;
    }
    
    .front{
        left: calc(var(--length) - 3.4rem);
        top: calc(-1 * var(--arrow-size) + 0.5 * var(--width));
        transform: rotate(-45deg);
        -webkit-transform: rotate(-45deg);
    }
    
    .back{
        left: 2.4rem;
        top: calc(-1 * var(--arrow-size));
        transform: rotate(135deg);
        -webkit-transform: rotate(135deg);
    }
</style>
