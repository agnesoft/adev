<template>
    <div class="element"  @mouseenter="showData(true,$event)" @mouseleave="showData(false,$event)">
        {{ element.id }}
    </div>
</template>

<script>
export default {
    name: "Element",
    props: {
        element: Object,
    },
    methods: {        
        showData(show,event){
            if(show){
                this.$emit('showData',[this.element],event);
            } else {
                this.$emit('showData',false,event);
            }
        }
    }
}
</script>

<style scoped>
    .element{
        width: 100%;
        height: 100%;
    }

</style>
