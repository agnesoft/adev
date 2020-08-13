<template>
    <div class="elements"  @mouseenter="showData(true,$event)" @mouseleave="showData(false,$event)">
    </div>
</template>

<script>
export default {
    name: "Element",
    props: {
        elements: Array,
    },
    methods: {        
        showData(show,event){
            if(show){
                this.$emit('showData',this.elements,event);
            } else {
                this.$emit('showData',false,event);
            }
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
