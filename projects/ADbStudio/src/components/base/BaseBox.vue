<template>
    <div class="base-box">
        <div class="header" @dblclick="toggleOpened()">
            <div class="title">
                <slot name="title" /> 
            </div>
            <BaseButton @click="toggleOpened()" class="btn-light toggle-button" :class="{opened: opened}" v-if="closable">
                <BaseIcon icon_key="arrow" />
            </BaseButton>
            
            <slot name="controls" />
        </div>
        <SlideYTransition :opened="opened || !closable">
            <div class="content" >
                <slot name="content"></slot>
            </div>
        </SlideYTransition>
    </div>
</template>

<script>
import SlideYTransition from "../transitions/SlideYTransition";

export default {
    name: "BaseBox",
    components: {
        SlideYTransition
    },
    props: {
      closable: {
        type: Boolean,
        default: true
      },
      def_opened: {
        type: Boolean,
        default: false
      }
    },
    data() {
        return {
            opened: this.def_opened,
            scroll: false,
        }
    },
    methods: {
        toggleOpened(){
            this.opened = !this.opened;
            if(this.opened){
                this.scroll=true;
            }
        }
    },
    updated(){
        if(this.scroll){
            this.$emit('opened');
            this.scroll = false;
        }
    }
}
</script>

<style scoped>
    .base-box{
        margin:  0.4rem 0.2rem;
        border: solid var(--border-width) var(--border-color,var(--light-primary-color));
        padding: 0;
        box-shadow:  1px 0 3px var(--primary-color);
        --button-size: 1.44rem;
    }
    .header{
        display: flex;
        justify-content: space-between;
        --icon-color: var(--primary-color);
        line-height: var(--button-size);
        padding: 0.4rem;
        border-bottom: solid 1px var(--border-color,var(--light-primary-color));
    }
    .header-fill .header{
        background: var(--border-color,var(--light-primary-color));
        padding: 0;
    }
    .header .title{
        flex-grow: 2;
        margin: 0;
        color: var(--primary-color);
    }
    .toggle-button, .controls a, .controls div, .controls button{
        display: block;
        overflow: hidden;
        height: var(--button-size);
        width: var(--button-size);
        transition: transform var(--menu-speed);
        padding: 0;
    }
    .controls{
        display: flex;
        justify-content: flex-end;

    }
    .controls a:hover{
        opacity: 0.7;
    }
    .opened{
       transform: rotate(180deg); 
    }
    .base-box h4{
        margin: 0;
    }
</style>
