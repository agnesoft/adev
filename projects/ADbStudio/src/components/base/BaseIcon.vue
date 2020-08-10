<template>
    <svg class="base-icon" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="100%" height="100%"  >
        <use v-if="isIcon('nodes')" xlink:href="../../assets/images/icons.svg#nodes" x = 0 y = 0></use>   
        <use v-else-if="isIcon('edges')" xlink:href="../../assets/images/icons.svg#edges" x = 0 y = 0></use> 
        <use v-else-if="isIcon('data')" xlink:href="../../assets/images/icons.svg#data" x = 0 y = 0></use>   
        <use v-else-if="isIcon('close')" xlink:href="../../assets/images/icons.svg#close" x = 0 y = 0></use>     
    </svg>
</template>

<script>
export default {
    name: 'BaseIcon',
    props: {
        icon_key: String,
    },
    methods: {
        isIcon(icon_key){
            return this.icon_key == icon_key;
        }
    }

}
</script>

<style scoped>
    /* .base-icon{
        --icon-size: var(--button-size);
        width: var(--icon-width, var(--icon-size));
        height: var(--icon-height, var(--icon-size));
    } */

</style>
