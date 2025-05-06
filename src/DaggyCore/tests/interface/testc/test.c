#include <stdio.h>
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

void sleep_ms(int milliseconds)
{
    #ifdef WIN32
        Sleep(milliseconds);
    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #else
        usleep(milliseconds * 1000);
    #endif
}

int quit_after_time(void* msec)
{
    sleep_ms(*(int*)(msec));
    libdaggy_app_stop();
    return 0;
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
    int time = 5000;
    libdaggy_run_in_thread(quit_after_time, &time);
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
    printf("Command %s in provider %s has stream from session %s: %llu\n", command_id, provider_id, stream.session, stream.seq_num);
}

void on_command_error(DaggyCore core, const char* provider_id, const char* command_id, DaggyError error)
{
    printf("Command %s in provider %s has error. Code: %d, Category: %s\n", command_id, provider_id, error.error, error.category);
}
