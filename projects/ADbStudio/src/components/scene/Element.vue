<template>
    <div class="element"  @mouseenter="showData(true,$event)" @mouseleave="showData(false,$event)" @click="openDataBox()">
        {{ element.id }}
    </div>
</template>

<script>
import { mapActions } from 'vuex';
export default {
    name: "Element",
    props: {
        element: Object,
    },
    methods: {   
        ...mapActions({
            fetchElementData: 'scene/fetchElementData',
        }),     
        showData(show,event){
            if(show){
                this.$emit('showData',[this.element],event);
            } else {
                this.$emit('showData',false,event);
            }
        },
        openDataBox(){
            this.fetchElementData({id: this.element.id});
        }
    },
}
</script>

<style scoped>
    .element{
        width: 100%;
        height: 100%;
    }

</style>
