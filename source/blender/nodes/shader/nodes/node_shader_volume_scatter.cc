/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright 2005 Blender Foundation. All rights reserved. */

#include "node_shader_util.hh"

#include "UI_interface.h"
#include "UI_resources.h"

namespace blender::nodes::node_shader_volume_scatter_cc {

static void node_declare(NodeDeclarationBuilder &b)
{
  b.is_function_node();
  b.add_input<decl::Color>(N_("Color")).default_value({0.8f, 0.8f, 0.8f, 1.0f});
  b.add_input<decl::Float>(N_("Density")).default_value(1.0f).min(0.0f).max(1000.0f);
  b.add_input<decl::Float>(N_("Anisotropy"))
      .default_value(0.0f)
      .min(-1.0f)
      .max(1.0f)
      .subtype(PROP_FACTOR);
  b.add_input<decl::Float>(N_("Weight")).unavailable();
  b.add_output<decl::Shader>(N_("Volume"));
}

static void node_shader_buts_volume_scatter(uiLayout *layout, bContext *UNUSED(C), PointerRNA *ptr)
{
  uiItemR(layout, ptr, "phase_function", UI_ITEM_R_SPLIT_EMPTY_NAME, "", ICON_NONE);
}

static void node_shader_init_volume_scatter(bNodeTree *UNUSED(ntree), bNode *node)
{
  node->custom1 = VOLUME_SCATTER_HENYEY;
}

// static const char *gpu_shader_get_name(int mode)
// {
//   switch (mode) {
//     case VOLUME_SCATTER_RAYLEIGH:
//       return "volume_scatter_rayleigh";
//     case VOLUME_SCATTER_MEI:
//       return "volume_scatter_mei";
//     case VOLUME_SCATTER_HENYEY:
//       return "volume_scatter_henyey";
//   }

//   return nullptr;
// }


// class SocketSearchOp {
//  public:
//   std::string socket_name;
//   NodeVectorMathOperation mode = NODE_VECTOR_MATH_ADD;
//   void operator()(LinkSearchOpParams &params)
//   {
//     bNode &node = params.add_node("ShaderNodeVolumeScattering");
//     node.custom1 = mode;
//     params.update_and_connect_available_socket(node, socket_name);
//   }
// };

// static int sh_node_mix_ui_class(const bNode *node)
// {
//   const NodeShaderVolumeScatter &storage = node_storage(*node);
//   const eNodeSocketDatatype data_type = static_cast<eNodeSocketDatatype>(storage.data_type);

//   switch (data_type) {
//     case SOCK_RAYLEIGH:
//       return NODE_CLASS_OP_RAYLEIGH;
//     case SOCK_MIE:
//       return NODE_CLASS_OP_MIE;
//     default:
//       return NODE_CLASS_HENYEY;
//   }
// }

static void node_shader_update_volume_scatter(bNodeTree *ntree, bNode *node)
{
  bNodeSocket *sock_anisotropy = nodeFindSocket(node, SOCK_IN, "Anisotropy");
  nodeSetSocketAvailability(ntree, sock_anisotropy, ELEM(node->custom1, VOLUME_SCATTER_HENYEY, VOLUME_SCATTER_MEI));
}

// static void node_mix_gather_link_searches(GatherLinkSearchOpParams &params)
// {
//   const eNodeSocketDatatype sock_type = static_cast<eNodeSocketDatatype>(
//       params.other_socket().type);

//   if (params.in_out() == SOCK_OUT) {
//     params.add_item(IFACE_("Result"), [type](LinkSearchOpParams &params) {
//       bNode &node = params.add_node("ShaderVolumeScatter");
//       node_storage(node).data_type = type;
//       params.update_and_connect_available_socket(node, "Result");
//     });
//   }
//   else {
//     if (ELEM(sock_type, SOCK_HENYEY, SOCK_MIE)) {
//       params.add_item(IFACE_("Anisotropy"), [](LinkSearchOpParams &params) {
//         bNode &node = params.add_node("ShaderNodeMix");
//         node_storage(node).data_type = SOCK_VECTOR;
//         node_storage(node).factor_mode = NODE_MIX_MODE_NON_UNIFORM;
//         params.update_and_connect_available_socket(node, "Factor");
//       });
//     }
//     params.add_item(IFACE_("Factor"), [type](LinkSearchOpParams &params) {
//       bNode &node = params.add_node("ShaderNodeMix");
//       node_storage(node).data_type = type;
//       params.update_and_connect_available_socket(node, "Factor");
//     });
//     params.add_item(IFACE_("A"), [type](LinkSearchOpParams &params) {
//       bNode &node = params.add_node("ShaderNodeMix");
//       node_storage(node).data_type = type;
//       params.update_and_connect_available_socket(node, "A");
//     });
//     params.add_item(IFACE_("B"), [type](LinkSearchOpParams &params) {
//       bNode &node = params.add_node("ShaderNodeMix");
//       node_storage(node).data_type = type;
//       params.update_and_connect_available_socket(node, "B");
//     });
//   }
// }

static int node_shader_gpu_volume_scatter(GPUMaterial *mat,
                                          bNode *node,
                                          bNodeExecData *UNUSED(execdata),
                                          GPUNodeStack *in,
                                          GPUNodeStack *out)
{
  return GPU_stack_link(mat, node, "node_volume_scatter", in, out);
}

}  // namespace blender::nodes::node_shader_volume_scatter_cc

/* node type definition */
void register_node_type_sh_volume_scatter()
{
  namespace file_ns = blender::nodes::node_shader_volume_scatter_cc;

  static bNodeType ntype;

  sh_node_type_base(&ntype, SH_NODE_VOLUME_SCATTER, "Volume Scatter", NODE_CLASS_SHADER);
  ntype.declare = file_ns::node_declare;
  ntype.draw_buttons = file_ns::node_shader_buts_volume_scatter;
  node_type_init(&ntype, file_ns::node_shader_init_volume_scatter);
  node_type_gpu(&ntype, file_ns::node_shader_gpu_volume_scatter);
  node_type_update(&ntype, file_ns::node_shader_update_volume_scatter);

  nodeRegisterType(&ntype);
}
