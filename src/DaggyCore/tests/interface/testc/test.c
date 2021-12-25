#include <stdio.h>
#include <pthread.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <DaggyCore/Core.h>

const char* json_data =
"{\
    \"sources\": {\
        \"localhost\" : {\
            \"type\": \"local\",\
            \"commands\": {\
                \"ping1\": {\
                    \"exec\": \"ping 127.0.0.1\",\
                    \"extension\": \"log\"\
                },\
                \"ping2\": {\
                    \"exec\": \"ping 127.0.0.1\",\
                    \"extension\": \"log\"\
                    }\
            }\
        }\
    }\
}"
;

void* quit_after_time(void* seconds)
{
    sleep(*(unsigned int*)(seconds));
    libdaggy_app_stop();
    return NULL;
}

void on_daggy_state_changed(DaggyCore core, DaggyStates state);

void on_provider_state_changed(DaggyCore core, const char* provider_id, DaggyProviderStates state);
void on_provider_error(DaggyCore core, const char* provider_id, DaggyError error);

void on_command_state_changed(DaggyCore core, const char* provider_id, const char* command_id, DaggyCommandStates state, int exit_code);
void on_command_stream(DaggyCore core, const char* provider_id, const char* command_id, DaggyStream stream);
void on_command_error(DaggyCore core, const char* provider_id, const char* command_id, DaggyError error);

int main(int argc, char** argv)
{
    DaggyCore core;
    libdaggy_app_create(argc, argv);
    libdaggy_core_create(json_data, Json, &core);
    libdaggy_connect_aggregator(core,
                                on_daggy_state_changed,
                                on_provider_state_changed,
                                on_provider_error,
                                on_command_state_changed,
                                on_command_stream,
                                on_command_error);
    libdaggy_core_start(core);
    pthread_t thread;
    unsigned int time = 5;
    pthread_create(&thread, NULL, quit_after_time, &time);
    return libdaggy_app_exec();
}

void on_daggy_state_changed(DaggyCore core, DaggyStates state)
{
    printf("Daggy state changed: %d\n", state);
}

void on_provider_state_changed(DaggyCore core, const char* provider_id, DaggyProviderStates state)
{
    printf("Provider %s state changed: %d\n", provider_id, state);
}

void on_provider_error(DaggyCore core, const char* provider_id, DaggyError error)
{
    printf("Provider %s error. Code: %d, Category: %s\n", provider_id, error.error, error.category);
}

void on_command_state_changed(DaggyCore core, const char* provider_id, const char* command_id, DaggyCommandStates state, int exit_code)
{
    printf("Command %s in provider %s state changed: %d\n", command_id, provider_id, state);
}

void on_command_stream(DaggyCore core, const char* provider_id, const char* command_id, DaggyStream stream)
{
    printf("Command %s in provider %s has stream: %li\n", command_id, provider_id, stream.seq_num);
}

void on_command_error(DaggyCore core, const char* provider_id, const char* command_id, DaggyError error)
{
    printf("Command %s in provider %s has error. Code: %d, Category: %s\n", command_id, provider_id, error.error, error.category);
}
