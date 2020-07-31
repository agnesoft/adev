<template>
    <div class="scene">
        <form class="command-form" @submit.prevent="sendCommand">
            <textarea class="command" v-model.trim="command" :placeholder="$t('write-command')" rows="1" />
            <input type="submit" :value="$t('run')" class="submit-button">
        </form>
        <ADbView id="adb-view" />
        <SearchField id="search-field" />
    </div>
</template>

<script>
import SearchField from '@/components/scene/SearchField.vue'
import ADbView from '@/components/scene/ADbView.vue'
import { mapActions } from 'vuex'

export default {
    name: "Scene",
    components: {
        SearchField,
        ADbView
    },
    data(){
        return {
            command: '',
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
        }
    }
}
</script>

<style scoped>
    .scene{
        position: relative;
        display: grid;
        grid-template-columns: 1fr;
        grid-template-rows: max-content minmax(max-content,1fr);
        grid-template-areas: 
            'command'
            'view'
            ;
    }
    .command-form{
        grid-area: command;
        display: flex;
        max-width: 80rem;
    }
    .command{
        flex-grow: 2;
        /* max-width: 50rem */
    }
    .adb-view{
        grid-area: view;
    }
    .search-field{
        grid-area: search;
    }
</style>
