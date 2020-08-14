<template>
    <SlideXTransition :opened="elements.length > 0"  >
        <div class="right-panel">
            <div>
                <BaseButton class="btn-primary" @click="removeAllElements">
                    {{ $t('hide-all-data') }}
                </BaseButton>
                
                <transition-group name="fade" >
                    <BaseBox  v-for="element in elements" :key="element.id" :def_opened="true" :closable="true">
                        <h4 slot="title">{{ getTitle(element) }}</h4>
                        <div class="controls" slot="controls">
                            <BaseButton class="btn-light" @click="closeBox(element.id)">
                                <svg width="100%" height="100%">
                                    <use xlink:href="../../assets/images/icons.svg#close" x = 0 y = 0></use>
                                </svg>
                            </BaseButton>
                        </div>
                        <ElementData slot="content" v-if="dataValid(element)" :data="element.data" />
                        <div slot="content" v-else class="no-data">
                            {{ $t("no-data-selected") }}
                        </div>
                    </BaseBox>
                </transition-group>
            </div>
        </div>
    </SlideXTransition>     
</template>

<script>
import SlideXTransition from "../transitions/SlideXTransition";
import ElementData from './ElementData';
import { mapActions,mapGetters } from 'vuex';

export default {
    name: "RightPanel",
    components: {
        SlideXTransition,
        ElementData,
    },
    data(){
        return {
        }
    },
    computed: {
        ...mapGetters({
            elements: 'scene/getElementsData',
        }),
    },
    methods: {
        ...mapActions({
            removeElement: 'scene/removeElementData',
            removeAllElements: 'scene/removeAllElementsData',
        }),
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
        closeBox(id){
            this.removeElement({id: id});
        }
    }
}
</script>

<style scoped>
    .fade-enter-active {
        transition: all 0.3s
    }
    .fade-enter {
        opacity: 0;
    }
    .fade-leave-active {
        transition: all 0.3s;
        opacity: 0;
    }
    .right-panel{
        max-width: 0.3vw;
        min-width: 12rem;
        max-height: 100%;
        position: relative;
        overflow: auto;
    }
    .right-panel>div{
        background: white;
        position: absolute;
        top: 0;
        right: 0;
        width: 100%;
    }

</style>
