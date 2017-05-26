float4 gTransform[ 4 ] : register( c0 );
float4 gWorldMatrix[ 3 ] : register( c4 );
float4 gInvTransposedWorldMatrix[ 3 ] : register( c7 );
float4 gDiffuseColor : register( c10 );
float3 gAmbient : register( c11 );
float4 gLightPosX : register( c12 );
float4 gLightPosY : register( c13 );
float4 gLightPosZ : register( c14 );
float3 gLightColor0 : register( c15 );
float3 gLightColor1 : register( c16 );
float3 gLightColor2 : register( c17 );
float3 gLightColor3 : register( c18 );

struct ToPixel{
	float4 screenPosition : POSITION;
	float4 color : COLOR0; //頂点カラー
	float2 texCoord : TEXCOORD0; //テクスチャ座標
};

ToPixel main( 
float4 aPosition : POSITION0,
float3 aNormal : NORMAL0,
float4 aColor : COLOR0,
float2 aTexCoord : TEXCOORD0 ){
	ToPixel o;
	o.screenPosition.x = dot( gTransform[ 0 ], aPosition );
	o.screenPosition.y = dot( gTransform[ 1 ], aPosition );
	o.screenPosition.z = dot( gTransform[ 2 ], aPosition );
	o.screenPosition.w = dot( gTransform[ 3 ], aPosition );
	float3 wp; //ワールド位置
	wp.x = dot( gWorldMatrix[ 0 ], aPosition );
	wp.y = dot( gWorldMatrix[ 1 ], aPosition );
	wp.z = dot( gWorldMatrix[ 2 ], aPosition );
	float3 wn; //ワールド法線
	wn.x = dot( gInvTransposedWorldMatrix[ 0 ].xyz, aNormal );
	wn.y = dot( gInvTransposedWorldMatrix[ 1 ].xyz, aNormal );
	wn.z = dot( gInvTransposedWorldMatrix[ 2 ].xyz, aNormal );
#ifdef LINEAR_ZBUFFER
	o.screenPosition.z *= o.screenPosition.w;
#endif

	float4 lvX, lvY, lvZ;
	lvX = gLightPosX - wp.xxxx;
	lvY = gLightPosY - wp.yyyy;
	lvZ = gLightPosZ - wp.zzzz;
	float4 lvL;
	lvL = lvX * lvX;
	lvL += lvY * lvY;
	lvL += lvZ * lvZ;
	lvL += 0.00001; //0割り防止
	float4 d = lvX * wn.xxxx;
	d += lvY * wn.yyyy;
	d += lvZ * wn.zzzz;
	d /= lvL;
	d = max( ( float4 )0, d ); //マイナスは0。
	float3 c;
	c = gAmbient.xyz;
	c += gLightColor0 * d.x;
	c += gLightColor1 * d.y;
	c += gLightColor2 * d.z;
	c += gLightColor3 * d.w;
	o.color.xyz = c * aColor.zyx * gDiffuseColor.xyz; //RGB入れ替え
	o.color.w = aColor.w * gDiffuseColor.w; //頂点アルファも乗算
	o.texCoord = aTexCoord;
	return o;
}

