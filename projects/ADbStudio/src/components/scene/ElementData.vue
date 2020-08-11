<template>
    <FadeTransition :opened="show">  
        <table v-if="dataValid" class="element-data" :style="dataStyle">
            <tr v-for="(value, data_key) in data" :key="data_key">
                <td>{{ data_key }}</td>
                <td>{{ value }}</td>
            </tr>
        </table>
        <div v-else class="element-data" :style="dataStyle">
            {{ $t("no-data-selected") }}
        </div>
    </FadeTransition>
</template>

<script>
import FadeTransition from "@/components/transitions/FadeTransition";

export default {
    name: "ElementData",
    components: {
        FadeTransition,
    },
    props: {
        show: {
            type: Boolean,
            default: true,
        },
        data: Object,
        dataStyle: {
            type: Object,
            default() {return {}},
        }
    },
    computed: {
        dataValid(){
            if(Object.keys(this.data).length != 0){
                return true;
            }
            return false;
        }
    }
}
</script>

<style scoped>
    .element-data{
        background: var(--light-color);
        box-shadow:  0 0 3px var(--primary-color), 2px 2px 5px var(--dark-color);
        /* border-radius: 0.4rem; */
        border: solid 1px var(--primary-color);
        padding: 0.4rem;
        width: 12rem;
        position: relative;
        z-index: 1000;
    }
    .element-data td,.element-data th {
        border: 1px solid #dddddd;
        text-align: left;
        padding: 0.4rem 0.8rem;
    }
    .element-data tr:nth-child(odd) {
        background-color: var(--light-color);
    }
    .element-data tr:nth-child(even) {
        background-color: #dddddd;
    }
    .element-data tr:nth-child(even) td{
        border: 1px solid var(--light-color);
    }
    table.element-data {
        border-collapse: collapse;
        border-style: hidden;
    }

</style>
