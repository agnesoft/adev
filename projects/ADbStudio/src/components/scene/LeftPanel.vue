<template>

    <SlideXTransition :opened="has_data"  >
        <div class="left-panel">
            <div class="tabs">
                <div class="tab-button" :class="{active: tab.key==active_tab_key}" v-for="tab in tabs" :key="tab.key" :title="tab.title" >
                    <BaseButton class="btn-circle" :class="{'btn-primary' : tab.key==active_tab_key,'btn-light' : tab.key!=active_tab_key}" @click="buttonClicked(tab.key)">
                        <BaseIcon :icon_key="tab.key" />
                    </BaseButton>
                </div>
            </div>
            <SlideXTransition :opened="active_tab_key != -1"  >
                <div class="tab-box">
                    <header class="tab-header">
                        {{tab_title}}
                    </header>
                    <transition name="fade" mode="out-in">
                        <component class="tab-content" v-bind:is="component_content"  v-on="$listeners"></component>
                    </transition>
                </div>
            </SlideXTransition>    
        </div>
    </SlideXTransition> 
</template>

<script>
import SlideXTransition from "../transitions/SlideXTransition";
import { mapGetters } from 'vuex'

export default {
    name: "LeftPanel",
    components: {
        SlideXTransition,
        NodeListData: () => import('./NodeListData'),
        EdgeListData: () => import('./EdgeListData'),
        RawData: () => import('./RawData')
    },
    data() {
        return {
            active_tab_key: -1,
            tabs: [
                {
                    key: 'nodes',
                    title: this.$t('list-of-nodes'),
                    component_content: "NodeListData",
                },
                {
                    key: 'edges',
                    title: this.$t('list-of-edges'),
                    component_content: "EdgeListData",
                },
                {
                    key: 'data',
                    title: this.$t('raw-data'),
                    component_content: "RawData",
                },
            ]
        }
    },
    computed: {
        ...mapGetters({
            data: 'scene/getData',
        }),
        active_tab(){
            if(!this.active_tab_key){
                return false;
            }
            let active_tab_key = this.active_tab_key;
            let tab = this.tabs.find((elem) => elem.key == active_tab_key);
            if(!tab){
                return false;
            }
            return tab;
        },
        component_content(){
            let active_tab_key = this.active_tab_key;
            if(active_tab_key){
                return this.active_tab.component_content;
            }
            return false;
        },
        tab_title(){
            let active_tab_key = this.active_tab_key;
            if(active_tab_key){
                return this.active_tab.title;
            }
            return '';
        },
        has_data(){
            return !(Object.keys(this.data).length === 0 && this.data.constructor === Object);
        }
    },
    methods: {
        buttonClicked(tab_key){
            let tab = this.tabs.find((elem) => elem.key == tab_key);
            if(!tab){
                return;
            }
            if(this.active_tab_key === tab_key){
                this.active_tab_key = -1;
            } else {
                this.active_tab_key = tab_key;
            }
        },
    }
}
</script>

<style scoped>
    .left-panel{
        display: flex;
    }
    .tabs{
        display: flex;
        flex-direction: column;
        margin-right: var(--grid-gap);
    }
    .btn{
        margin: 0.2rem;
        transition: all 0.2s ease;
    }
    .tab-box{
        border-radius: 0.4rem;
        overflow: hidden;
        background: white;
        display: flex;
        flex-direction: column;
        max-height: 80vh;
        --shadow: var(--dark-color) 1px 1px 5px 0px;
        -webkit-box-shadow:  var(--shadow);
        -moz-box-shadow:  var(--shadow);
        box-shadow:  var(--shadow);
    }
    .tab-header{
        padding: 0.5rem;
        white-space: nowrap;
    }
    .tab-content{
        width: 15rem;
        padding: 0.5rem;
        margin: 0;
        height: 100%;
        overflow: auto;
    }
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
    .tab-header{
        background: var(--primary-color);
        color: var(--light-color);
    }
</style>
