<template>
    <div class="search-field" :class="{opened: showContent}">
        <SlideXTransition :opened="showContent">
            <form class="search-form" @submit.prevent="onSubmit">
                <input type="text" ref="search_input" class="search-input" v-model.trim="search" :placeholder="$t('search-element')"/>
            </form>
        </SlideXTransition>
        <BaseButton class="btn-light" @click="handleClick">
            <svg width="100%" height="100%">
                <use xlink:href="../../assets/images/icons.svg#search" x = 0 y = 0></use>
            </svg>
        </BaseButton>
    </div>
</template>

<script>
import SlideXTransition from "@/components/transitions/SlideXTransition";
export default {
    name: "SearchField",
    components: {
        SlideXTransition,
    },
    data(){
        return {
            search: '',
            showContent: false,
        }
    },
    methods: {
        handleClick(){
            if(this.search != ''){
                this.onSubmit();
                return;
            } 
            this.showContent = !this.showContent; 
            if(this.showContent){
                let component = this;
                this.$nextTick()
                .then(function () {
                    component.$refs.search_input.focus();
                })
            }
        },
        // handleFocusOut(){
        //     this.showContent = false;
        // },
        onSubmit(){
            this.$emit('search',this.search);
            this.search = ''
        }
    },
}
</script>

<style scoped>
    .search-field{
        display: flex;
        height: var(--button-size);
        border-radius: calc(var(--button-size) * 0.5);
        transition: border var(--transition-ease);
    }
    .search-field.opened{
        border: 1px solid var(--light-primary-color);
    }
    .btn-light{
        width: var(--button-size);
        height: var(--button-size);
        padding: 0.2rem;
    }
    .base-expandable{
        z-index: 1;
        margin: 0.2rem;
        /* --shadow: var(--primary-color) 0px 0px 2px 2px;
        -webkit-box-shadow:  var(--shadow);
        -moz-box-shadow:  var(--shadow);
        box-shadow:  var(--shadow); */
        display: inline-block;
    }
    .search-input{
        border: none;
        padding-left: 1rem;
        background: none;
        height: 100%;
    }
</style>
