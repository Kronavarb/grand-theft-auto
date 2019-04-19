;	c14		0.0 0.5 1.0 2.0
;	c26		specular
;	c27		spec, fresnel, 0.6, power

 dp4	r3.x, v0, c4
 dp4	r3.y, v0, c5
 dp4	r3.z, v0, c6
 add	r2.xyz, c12.xyzz, -r3.xyzz
 dp3	r0.x, v1.xyzz, c4.xyzz
 dp3	r2.w, r2.xyzz, r2.xyzz
 +mov	oD0.w, c14.z
 dp3	r0.y, v1.xyzz, c5.xyzz
 dp3	r0.z, v1.xyzz, c6.xyzz
 +rsq	r1.w, r2.w
 mov	r5.w, c14.w
 mul	r2.xyz, r2.xyzz, r1.w		; r2: normalized view vector
 dp3	r0.w, r0.xyzz, r0.xyzz
 add	r4.xyz, r2.xyzz, -c13.xyzz
 mov	r11.w, c27.w
 +rsq	r1.w, r0.w
 dp3	r4.w, r4.xyzz, r4.xyzz
 mul	r0.xyz, r0.xyzz, r1.w		; r0: normalized world normal
 add	r10.xyz, r2.xyzz, -c18.xyzz
 mul	r11.z, c27.w, r5.w
 +rsq	r1.w, r4.w
 dp3	r4.w, r10.xyzz, r10.xyzz
 add	r9.yzw, r2.yyzx, -c19.yyzx
 mul	r4.xyz, r4.xyzz, r1.w
 dp3	r4.w, r9.wyzz, r9.wyzz
 +rsq	r1.w, r4.w
 add	r8.xzw, r2.yyzx, -c20.yyzx
 dp3	r10.w, r4.xyzz, r0.xyzz
 mul	r4.xyz, r10.xyzz, r1.w
 +rsq	r1.x, r4.w
 dp3	r4.w, r8.wxzz, r8.wxzz
 add	r10.xyz, r2.xyzz, -c21.xyzz
 max	r11.x, r10.w, c14.x
 +rsq	r1.w, r4.w
 dp3	r10.w, r4.xyzz, r0.xyzz
 mul	r4.xyz, r9.wyzz, r1.x
 +lit	r1.z, r11.xxxw
 dp3	r4.w, r10.xyzz, r10.xyzz
 max	r11.y, r10.w, c14.x
 dp3	r11.x, r4.xyzz, r0.xyzz
 mul	r4.xyz, r8.wxzz, r1.w
 +rsq	r1.x, r4.w
 mul	r8.xyz, r1.z, c26.xyzz			; direct light
 mul	r11.w, c27.w, r5.w
 +lit	r1.z, r11.yyyz
 max	r11.y, r11.x, c14.x
 dp3	r11.x, r4.xyzz, r0.xyzz
 mul	r4.xyz, r10.xyzz, r1.x
 dp4	r6.y, v0, c0
 dp4	r6.z, v0, c1
 dp4	r6.x, v0, c2
 +dp4	oFog.x, v0, c2
 mad	r8.xyz, r1.z, c22.xyzz, r8.xyzz		; add extra 1
 mul	r11.w, c27.w, r5.w
 +lit	r1.z, r11.yyyw
 max	r11.y, r11.x, c14.x
 +mov	oPos.xyz, r6.yzxw
 dp3	r11.x, r4.xyzz, r0.xyzz
 dp4	oPos.w, v0, c3
 mad	r8.xyz, r1.z, c23.xyzz, r8.xyzz		; add extra 2
 mul	r11.w, c27.w, r5.w
 +lit	r1.z, r11.yyyw
 max	r11.y, r11.x, c14.x
+rcc	r1.x, r12.w
 mad	r8.xyz, r1.z, c24.xyzz, r8.xyzz		; add extra 3
mul	oPos.xyz, r12.xyzz, c-38.xyzz
 +lit	r1.z, r11.yyyw
 mad	oD0.xyz, r1.z, c25.xyzz, r8.xyzz	; add extra 4
mad	oPos.xyz, r12.xyzz, r1.x, c-37.xyzz
