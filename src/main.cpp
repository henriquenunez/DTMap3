#include <iostream>

#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <ImGuiFileBrowser.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#include "shader.h"
#include "grid.hpp"
#include "part.hpp"
#include "part_data.hpp"
#include "colorbar.hpp"
#include "camera.hpp"
#include "importer.h"
#include "globals.h"

#include "shader.c"
#include "camera.cpp"

#include <iostream>
#include <chrono>

#include "defines.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

float ox = 0.0f, oy = 0.0f, oz = -20.0f;
bool has_moved = false;
bool orthogonal = false;
bool orthogonal_change = false;
glm::vec3 rotation_vector(1.0, 0.0, 0.0);
float rotation_angle = 0.0;

float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw = 0.0f;
float part_pitch = 0.0f, part_yaw = 0.0f;

bool taking_screenshot;

float fov = 20.0f;

std::string save_filename;

// For automatic mode
char *arg_filename;
char *arg_col_name;
float arg_min_param, arg_max_param;
int arg_screenshot_mode;
bool auto_screenshot;

//reference_arrows_t ref_arrows;

// DTMap parameter variables
float colormap_h_min, colormap_h_max;
bool crop_x, crop_y, crop_z;
float crop_x_amount, crop_y_amount, crop_z_amount;
float crop_x_min, crop_x_max;
#define MAX_COL_SIZE 128
char h_col_name[MAX_COL_SIZE];

// Resolution
float resolution_x, resolution_y, resolution_z;

bool imgui_cap_mouse; //Gambeta básica

Camera view_cam;

void save_framebuffer()
{
    void *data = malloc(3 * SCREEN_WIDTH * SCREEN_HEIGHT);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);

    if (save_filename.size() == 0) return;
    stbi_flip_vertically_on_write(true);
    stbi_write_png(save_filename.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT, 3, data, 3 * SCREEN_WIDTH);

    taking_screenshot = false;

    free(data);
}

PartRepresentation *a_part_view = NULL;
Colorbar *part_colorbar = NULL;

void loadPart(const char *filename)
{
    PartDataCSVImporter a_importer(filename, glm::vec3(resolution_x, resolution_y, resolution_z), h_col_name, colormap_h_min, colormap_h_max);

    PartData a_part = a_importer.imported_part_data; //generate_a_part();

    if (a_part_view != NULL) delete a_part_view;
    if (part_colorbar != NULL) delete part_colorbar;

    a_part_view = new PartRepresentation(a_part);
    part_colorbar = new Colorbar(colormap_h_min, colormap_h_max);
    a_part.dealloc();
}

int main(int argc, char *argv[])
{
    /******************* GL INITIALIZATION  **************/
    glfwInit();
    
#ifdef __APPLE__
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    
    //WINDOW CREATION
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DTMap3D", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to initialize GLFW" << std::endl;

        const char* description;
        int code = glfwGetError(&description);
         
        if (description)
            std::cout << code << ": " << description;
        
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(0);

    const GLubyte *vendor = glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte *renderer = glGetString(GL_RENDERER); // Returns a hint to the model

    printf("%s\n", vendor);
    printf("%s\n", renderer);
    //So triagles do not overlap improperly.

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    /***************  END GL INITIALIZATION  *************/

    //Setting up imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    bool show_demo_window = true;
    bool show_open_file_window = false;
    bool rotate_grid = false;
    bool show_reference_grid[3] = {0};

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    std::vector <ImVec4> color_pallete;

    // Imgui file browser.
    imgui_addons::ImGuiFileBrowser file_dialog;

    ReferenceGrid main_ref_grid_0(0);
    ReferenceGrid main_ref_grid_1(1);
    ReferenceGrid main_ref_grid_2(2);

    // Auto version
    if (argc >= 2)
    {
        sscanf(argv[3], "%f", &arg_min_param);
        sscanf(argv[4], "%f", &arg_max_param);
        sscanf(argv[5], "%d", &arg_screenshot_mode);

        printf("Screenshot mode: %d\n", arg_screenshot_mode);

        arg_filename = argv[1];
        arg_col_name = argv[2];

        strncpy(h_col_name, arg_col_name, 128);
        colormap_h_min = arg_min_param;
        colormap_h_max = arg_max_param;
        auto_screenshot = true;
        save_filename = std::string(argv[6]);
    }

    // No need for UI control.
    if (arg_screenshot_mode)
    {
        loadPart(arg_filename);
        taking_screenshot = true;

        view_cam.setDefault();

        if (arg_screenshot_mode == 2) // Setup crop Y
        {
            view_cam.setFrontView();
            crop_y = true;
            crop_y_amount = 9.75;
        }
    }

    // Actual part importer.
    auto start = std::chrono::steady_clock::now();

    int a = 0;
    //EVENT LOOP
    while (!glfwWindowShouldClose(window))
    {
        //std::cout << "yay\n";
        processInput(window);

        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 viewProjection = view_cam.getViewProjectionMatrix();

        // Visualization
        if (a_part_view != NULL)
        {
            a_part_view->setModelMat(view_cam.getPartMatrix());
            a_part_view->setViewProjectionMat(viewProjection);
            a_part_view->should_rotate = rotate_grid;

            a_part_view->set_crop_y(crop_y, crop_y_amount);
            a_part_view->render();
        }

        if (part_colorbar)
        {
            part_colorbar->render();
        }

        // GUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!taking_screenshot)
        {
            static int counter = 0;

            ImGui::Begin("HeatMAP3D Menu");

            ImGui::Checkbox("Rotate Grid", &rotate_grid);

            ImGui::Checkbox("Reference Grid X", &show_reference_grid[0]);
            ImGui::Checkbox("Reference Grid Y", &show_reference_grid[1]);
            ImGui::Checkbox("Reference Grid Z", &show_reference_grid[2]);

            ImGui::Checkbox("X cropping", &crop_x);
            if (crop_x)
            {
                ImGui::SameLine();
                ImGui::SliderFloat("> X", &crop_x_amount, 0.0f, 50.0f);
            }

            ImGui::Checkbox("Y cropping", &crop_y);
            if (crop_y)
            {
                ImGui::SameLine();
                ImGui::SliderFloat("> Y", &crop_y_amount, 0.0f, 50.0f);
            }

            ImGui::Checkbox("Z cropping", &crop_z);
            if (crop_z)
            {
                ImGui::SameLine();
                ImGui::SliderFloat("> Z", &crop_z_amount, 0.0f, 50.0f);
            }

            ImGui::InputText("H column", h_col_name, IM_ARRAYSIZE(h_col_name));
            ImGui::InputFloat("Min H", &colormap_h_min, 1.0f, 100.0f, "%.2f");
            ImGui::InputFloat("Max H", &colormap_h_max, 1.0f, 100.0f, "%.2f");

            ImGui::InputFloat("Res X", &resolution_x, 0.1f, 10.0f, "%.2f");
            ImGui::InputFloat("Res Y", &resolution_y, 0.1f, 10.0f, "%.2f");
            ImGui::InputFloat("Res Z", &resolution_z, 0.1f, 10.0f, "%.2f");

            // Setting all cutoffs
            if (a_part_view != NULL)
            {
                a_part_view->set_crop_x(crop_x, crop_x_amount);
                a_part_view->set_crop_y(crop_y, crop_y_amount);
                a_part_view->set_crop_z(crop_z, crop_z_amount);
            }

            if (ImGui::Button("Open File")) show_open_file_window = true;

            // Views
            if (ImGui::Button("Isometric view")) view_cam.setIsometric();
            ImGui::SameLine();
            if (ImGui::Button("Top view")) view_cam.setTopView();
            ImGui::SameLine();
            if (ImGui::Button("Side view")) view_cam.setSideView();
            ImGui::SameLine();
            if (ImGui::Button("Front view")) view_cam.setFrontView();
            if (ImGui::Button("Default view")) view_cam.setDefault();

            if (ImGui::Button("Screenshot")) taking_screenshot = true;
            ImGui::SameLine();
            if (ImGui::Button("Cam info")) view_cam.printInfo();

            ImGui::End();
        }

        imgui_cap_mouse = io.WantCaptureMouse ? true : false;

        if (show_open_file_window)
        {
            ImGui::OpenPopup("Open File");
        }

        if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310),
                                       ".csv"))
        {
            ox = oy = 0.0f;
            oz = -100.0f;

            // Here, try to load the part information.
            std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file

            // Actual part importer.
            auto start = std::chrono::steady_clock::now();

            save_filename = std::string(file_dialog.selected_path);
            save_filename += ".png";

            // Import part data here.
            try
            {
                loadPart(file_dialog.selected_path.c_str());
            } catch (...)
            {
                ImGui::OpenPopup("ImportError");
            }
            auto end = std::chrono::steady_clock::now();

            std::cout << "Elapsed time in nanoseconds : "
                      << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                      << " ns" << std::endl;
        }

        show_open_file_window = false;

        if (ImGui::BeginPopupModal("ImportError", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Error during import. Check if the CSV format and if it contains the columns.");
            if (ImGui::Button("Ok"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

        if (taking_screenshot && elapsed > 10)
        {
            printf("taking screenshot!\n");
            save_framebuffer();
            if (auto_screenshot) break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminating.
    if (a_part_view != NULL) delete a_part_view;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    view_cam.zoom((float) yoffset * 20.0f);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    viewport_changed = true;
    SCREEN_HEIGHT = height;
    SCREEN_WIDTH = width;
    glViewport(0, 0, width, height);
}

#define MOVE_SPEED 1
#define ANGLE_SPEED 0.06

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    const float rot_sensitivity = 0.005f;
    const float tran_sensitivity = 0.1f;

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    if (imgui_cap_mouse) return;

    // Rotation.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        view_cam.rotateDirection(xoffset * rot_sensitivity, false); // 2D rotation
        view_cam.rotateDirection(yoffset * rot_sensitivity, true); // 2D rotation
    }

    // Translation.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
    {
        view_cam.translate(glm::vec3(-xoffset, 0.0, -yoffset));
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_F1))
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else if (glfwGetKey(window, GLFW_KEY_F2))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    else if (glfwGetKey(window, GLFW_KEY_A))
    {
        has_moved = true;
        view_cam.translate(glm::vec3(MOVE_SPEED, 0.0, 0.0));
        ox -= MOVE_SPEED;
    }
    else if (glfwGetKey(window, GLFW_KEY_W))
    {
        has_moved = true;
        view_cam.translate(glm::vec3(0.0, -MOVE_SPEED, 0.0));
        oy += MOVE_SPEED;
    }
    else if (glfwGetKey(window, GLFW_KEY_S))
    {
        has_moved = true;
        view_cam.translate(glm::vec3(0.0, MOVE_SPEED, 0.0));
        oy -= MOVE_SPEED;
    }
    else if (glfwGetKey(window, GLFW_KEY_D))
    {
        has_moved = true;
        view_cam.translate(glm::vec3(-MOVE_SPEED, 0.0, 0.0));
        ox += MOVE_SPEED;
    }
    else if (glfwGetKey(window, GLFW_KEY_Q))
    {
        has_moved = true;
        view_cam.zoom(1.0f);
    }
    else if (glfwGetKey(window, GLFW_KEY_E))
    {
        has_moved = true;
        view_cam.zoom(-1.0f);
    }
    else if (glfwGetKey(window, GLFW_KEY_Z))
    {
        has_moved = true;
        rotation_angle += ANGLE_SPEED;
    }
    else if (glfwGetKey(window, GLFW_KEY_X))
    {
        has_moved = true;
        rotation_angle -= ANGLE_SPEED;
    }
}

#undef MOVE_SPEED
#undef ANGLE_SPEED

// unity build
//#include "importer.cpp"
//#include "colorbar.cpp"

