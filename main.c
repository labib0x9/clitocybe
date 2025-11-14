
void registration() {
    Register_Route("/health", "GET", health_handler);
}

int main() {

    registration();

    int err = server_start("tcp", "127.0.0.1:8080");
    if (err == -1) {
        return 1;
    }

    return 0;
}