<template>
    <div class="scene">
        <form class="command-form" @submit.prevent="sendCommand">
            <textarea class="command" v-model.trim="command" :placeholder="$t('write-command')" rows="1" />
            <input type="submit" :value="$t('run')" class="submit-button btn btn-primary">
        </form>
        <LeftPanel></LeftPanel>
        <div class="view-container" @wheel.prevent="onMouseWheel" @dragover.prevent @drop="onDrop" @dragstart="onDragStart">
            <ADbView id="adb-view" :zoom="zoom" draggable="true" :style="viewStyle"/>
        </div>
        <SearchField id="search-field" />
        <div class="controls">
            <BaseButton class="btn-default" @click="resetView">{{$t('reset-view')}}</BaseButton>
            <BaseButton class="btn-default" @click="zoomPlus">+</BaseButton>
            <BaseButton class="btn-default" @click="zoomMinus">-</BaseButton>
        </div>
    </div>
</template>

<script>
import SearchField from '@/components/scene/SearchField.vue'
import ADbView from '@/components/scene/ADbView.vue'
import LeftPanel from '@/components/scene/LeftPanel.vue'

import { mapActions } from 'vuex'

export default {
    name: "Scene",
    components: {
        SearchField,
        ADbView,
        LeftPanel
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
        },
        zoomPlus(){
            this.zoom++;
        },
        zoomMinus(){
            this.zoom--;
            this.zoom = this.zoom < -9 ? -9 : this.zoom;
        }
    },
    computed: {
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
        }
    }
}
</script>

<style scoped>
    .scene{
        position: relative;
        display: grid;
        grid-template-columns: max-content 1fr max-content;
        grid-template-rows: max-content 1fr max-content;
        grid-template-areas: 
            'command command command'
            'leftpanel view search'
            'leftpanel controls controls '
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
        grid-area: view / view / view / search;
        overflow: hidden;
        position: relative;
    }
    .search-field{
        grid-area: search;
        z-index: 1;

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
</style>
