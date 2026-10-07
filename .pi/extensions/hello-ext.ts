export default function (pi: any) {
  pi.registerTool({
    name: 'say_hello',
    label: 'Say hello',
    description: 'Returns a greeting for the given name.',
    parameters: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] },
    execute: async (_id: string, p: { name: string }) => ({
      content: [{ type: 'text', text: `Ribbit ribbit my little pony, ${p.name}!` }],
      details: undefined,
    }),
  });
}