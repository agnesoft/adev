<template>
    <div class="scene">
        <form class="command-form" @submit.prevent="sendCommand">
            <textarea class="command" v-model.trim="command" :placeholder="$t('write-command')" rows="1" />
            <input type="submit" :value="$t('run')" class="submit-button btn btn-primary">
        </form>
        <LeftPanel @showData="showData" />
        <div class="view-container" @wheel.prevent="onMouseWheel" @dragover.prevent @drop="onDrop" @dragstart="onDragStart">
            <ADbView id="adb-view" ref="adb_view" :zoom="zoom" draggable="true" :style="viewStyle" :searchedWord="searchedWord"  @showData="showData"/>
            <div class="search-wrap">    
                <SearchField @search="search"/>          
                <BaseMessage class="error" :opened="error === 'search'" @close="error=''">
                    {{ $t("element-not-found") }}
                </BaseMessage> 
                <BaseMessage class="info" :opened="searchedWord != ''" @close="resetView">
                    {{ $t('searched-element-word',{word: searchedWord}) }}
                </BaseMessage>
            </div>
        </div>
        <DataWrapper :show="displayData" :elements="elementsData" :dataStyle="elementDataStyle" @exploreData="exploreData" />
        <RightPanel />
        <div class="controls">
            <BaseButton class="btn-default reset" @click="resetView">{{$t('reset-view')}}</BaseButton>
            <BaseButton class="btn-default plus" @click="zoomPlus">+</BaseButton>
            <BaseButton class="btn-default minus" @click="zoomMinus">-</BaseButton>
        </div>
    </div>
</template>

<script>
import ADbView from '@/components/scene/ADbView.vue';
import LeftPanel from '@/components/scene/LeftPanel.vue';
import SearchField from '@/components/scene/SearchField.vue';
import DataWrapper from "@/components/scene/DataWrapper";
import RightPanel from "@/components/scene/RightPanel";

import { mapActions,mapGetters } from 'vuex';

export default {
    name: "Scene",
    components: {
        SearchField,
        ADbView,
        DataWrapper,
        LeftPanel,
        RightPanel
    },
    data(){
        return {
            command: '',
            zoom: 0,
            move: {
                x: 0,
                y: 0
            },
            startDragCoordinates: {
                x: 0,
                y: 0
            },
            searchedWord: '',
            error: '',
            dataShow: false,
            dataExporation: false,
            elementsData: [],
            dataPos: {
                x: 0,
                y: 0
            },
        }
    },
    methods: {
        ...mapActions({
            send_command: 'scene/sendCommand',
        }),
        sendCommand(){
            if(this.command != ""){
                this.send_command({command: this.command});
                this.command = '';
                this.resetView();
            }
        },
        onMouseWheel(event){
            event.deltaY < 0 ? this.zoomPlus() : this.zoomMinus(); 
        },
        onDragStart(event){
            this.startDragCoordinates.x = event.clientX;
            this.startDragCoordinates.y = event.clientY;
        },
        onDrop(event){
            this.move.x += (event.clientX - this.startDragCoordinates.x)/this.scale;
            this.move.y += (event.clientY - this.startDragCoordinates.y)/this.scale;
        },
        resetView(){
            this.move.x = 0;
            this.move.y = 0;
            this.zoom = 0;
            this.startDragCoordinates.x = 0;
            this.startDragCoordinates.y = 0;
            this.resetSearch();
        },
        resetSearch(){
            this.searchedWord = '';
            this.error = '';
        },
        zoomPlus(){
            this.zoom++;
        },
        zoomMinus(){
            this.zoom--;
            this.zoom = this.zoom < -9 ? -9 : this.zoom;
        },
        search(word){
            this.resetView();
            
            this.error = "search";
            this.searchedWord = '';
            if(isNaN(word) || word == 0){
                return false;
            }
            if(word < 0){
                let edge = this.edges.find((edge) => edge.id == word);
                if(edge){
                    this.searchedWord = word;
                    this.error = '';
                }
            } else if(word > 0) {
                let node = this.nodes.find((node) => node.id == word);
                if(node){
                    this.searchedWord = word;
                    this.error = '';
                }
            } 
        },
        showData(elements,event) {
            //this.elementsData = [];
            if(elements === false || elements.length === 0){
                setTimeout(() => this.dataShow = false, 1000);
            } else {
                this.elementsData = elements;
                // if(Object.prototype.hasOwnProperty.call(element, 'data')){
                //     this.data = element.data;
                // }
                this.dataShow = true;
                this.dataPos.y = event.clientY - this.$el.getBoundingClientRect().top + 15;
                this.dataPos.x = event.clientX - this.$el.getBoundingClientRect().left + 15;
            }
        },
        exploreData(explore){
            this.dataExporation = explore;
        }
    },
    computed: {
        ...mapGetters({
            nodes: 'scene/getNodes',
            edges: 'scene/getEdges',
        }),
        viewStyle: function(){
            return {
                "--move-x": this.move.x.toString()+"px",
                "--move-y": this.move.y.toString()+"px", 
                '--scale': this.scale,
            }
        },
        scale: function(){
            let scale = 1;
            scale = 1 + this.zoom/10;
            scale = scale < 0.1 ? 0.1 : scale;
            return scale;
        },
        elementDataStyle(){
            return {
                top: this.dataPos.y+"px",
                left: this.dataPos.x+"px"
            }
        },
        displayData(){
            return this.dataShow || this.dataExporation;
        }

    }
}
</script>

<style scoped>
    .scene{
        position: relative;
        display: grid;
        grid-template-columns: max-content 1fr max-content;
        grid-template-rows: max-content minmax(0, 1fr) max-content;
        grid-template-areas: 
            'command command command'
            'leftpanel view rightpanel'
            'leftpanel controls rightpanel'
            ;
        grid-gap: 0.4rem;
    }
    .command-form{
        grid-area: command;
        display: flex;
        max-width: 80rem;
    }
    .command{
        flex-grow: 2;
        resize: vertical;
        margin-right: 0.2rem;
        border: 1px solid #DDD;
        font: var(--primary-font);
        /* max-width: 50rem */
    }
    .view-container{
        grid-area: view;
        overflow: hidden;
        position: relative;
        background: white;
        box-shadow: inset  0 0 5px var(--light-color);
    }
    #adb-view{
        position: absolute;
    }
    .controls{
        grid-area: controls;
    }
    .controls>.btn{
        margin-right: 0.2rem;
    }
    .left-panel{
        grid-area: leftpanel;
    }
    
    .search-wrap{
        position: absolute;
        right: 0;
        top: 0;
        display: grid;
        grid-template-columns: 1fr;
        grid-template-rows: max-content max-content;
        grid-template-areas: 
            'input'
            'message'
            ;
        grid-gap: 0.4rem;
        /* flex-direction: column; */
        padding: 0.4rem;
        /* align-items: flex-end; */
    }
    .search-field{
        grid-area: input;
        place-self: center right;
    }
    .base-message{
        grid-area: message;
    }
    .data-wrapper{
        position: absolute;
    }
    .right-panel{
        grid-area: rightpanel;
    }
</style>
