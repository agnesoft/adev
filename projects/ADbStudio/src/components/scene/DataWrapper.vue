<template>
    <FadeTransition :opened="show">  
        <div class="data-wrapper"  :style="dataStyle"  @mouseenter="exploreData(true)" @mouseleave="exploreData(false)">
            <BaseBox  v-for="element in elements" :key="element.id" :def_opened="boxOpened" :closable="true">
                <h4 slot="title">{{ getTitle(element) }}</h4>
                <ElementData slot="content" v-if="dataValid(element)" :data="element.data" />
                <div slot="content" v-else class="no-data">
                    {{ $t("no-data-selected") }}
                </div>
            </BaseBox>
            
        </div>
    </FadeTransition>
</template>

<script>
import ElementData from './ElementData';
import FadeTransition from "@/components/transitions/FadeTransition";

export default {
    name: "DataWrapper",
    components: {
        ElementData,
        FadeTransition,
    },
    props: {
        elements: Array,
        show: {
            type: Boolean,
            default: true,
        },
        dataStyle: {
            type: Object,
            default() {return {}},
        }
    },
    computed:{
        boxOpened: function(){
            return this.elements.length === 1;
        }
    },
    methods: {
        dataValid(element){
            if(Object.prototype.hasOwnProperty.call(element, 'data') && Object.keys(element.data).length != 0){
                return true;
            }
            return false;
        },
        getTitle(element){
            if(element.id < 0){
                return this.$t('edge-id',{'id': element.id});
            } else {
                return this.$t('node-id',{'id': element.id});
            }
        },
        exploreData(explore){
            this.$emit('exploreData',explore);
        },
    }

}
</script>

<style scoped>
    .data-wrapper{
        background: var(--light-color);
        max-width: 0.7vw;
        min-width: 12rem;
        max-height: 30rem;
        z-index: 1000;
        box-shadow:  0 0 3px var(--primary-color), 2px 2px 5px var(--dark-color);
        overflow: auto;
    }

    .no-data{
        padding: 0.4rem;
    }
</style>
