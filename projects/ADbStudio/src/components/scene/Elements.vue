<template>
    <div class="elements"  @mouseenter="showData(true,$event)" @mouseleave="showData(false,$event)" @click="openDataBoxes()">
    </div>
</template>

<script>
import { mapActions } from 'vuex';
export default {
    name: "Element",
    props: {
        elements: Array,
    },
    methods: {     
        ...mapActions({
            fetchElementData: 'scene/fetchElementData',
        }),      
        showData(show,event){
            if(show){
                this.$emit('showData',this.elements,event);
            } else {
                this.$emit('showData',false,event);
            }
        },
        openDataBoxes(){
            this.elements.forEach((element) => this.fetchElementData({id: element.id}));
        }
    }
}
</script>

<style scoped>
    .elements{
        content: "";
        display: block;
        --size: 0.8rem;
        width: var(--size);
        height: var(--size);
        background-color: var(--secondary-color);
        border-radius: 50%;
        transition: all var(--transition-ease); 
        position: absolute;
    }

</style>
